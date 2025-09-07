#!/bin/bash

# Assign the argument to a variable, default to "false" if no argument is provided
COMPILE_ONLY=${1:-false}

# Get the directory of the current script
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Variables
IP="192.168.43.1:5555" #84741f7e6c219092
LIB_DIR="$SCRIPT_DIR/../../../../build"
TARGET_DIR="//data/local/tmp"

ANDROID_NDK_VERSION="21.4.7075529"
CMAKE_VERSION="3.31.4"

# Detect OS
OS="$(uname -s)"
case "$OS" in
    Linux*)     machine=Linux;;
    Darwin*)    machine=Mac;;
    CYGWIN*|MINGW*|MSYS*) machine=Windows;;
    *)          machine="UNKNOWN:${OS}"
esac

if [ "$machine" == "UNKNOWN:${OS}" ]; then
    echo "Unsupported OS: $OS"
    exit 1
fi

# Set Android SDK path based on OS
ADB=""
ANDROID_SDK=""
if [ "$machine" == "Linux" ] || [ "$machine" == "Mac" ]; then
  if [ -d "$HOME/Android/Sdk" ]; then
    ANDROID_SDK="$HOME/Android/Sdk"
    ADB="$ANDROID_SDK/platform-tools/adb"
  fi
elif [ "$machine" == "Windows" ]; then
  if [ -d "$USERPROFILE/AppData/Local/Android/Sdk" ]; then
    ANDROID_SDK="$USERPROFILE/AppData/Local/Android/Sdk"
    ADB="$ANDROID_SDK/platform-tools/adb.exe"
  fi
fi

# Check if ADB and ANDROID_SDK are set correctly
if [ -z "$ADB" ] || [ ! -x "$ADB" ]; then
  echo "ADB not found or not executable"
  exit 1
fi

if [ -z "$ANDROID_SDK" ] || [ ! -d "$ANDROID_SDK" ]; then
  echo "ANDROID SDK is not at the expected location"
  exit 1
fi

# Check if NDK directory exists
NDK_DIR="$ANDROID_SDK/ndk"
if [ ! -d "$NDK_DIR" ]; then
  echo "NDK is not installed at the expected location: $NDK_DIR"
  exit 1
fi

# Check if the correct NDK version is installed
ANDROID_NDK="$NDK_DIR/$ANDROID_NDK_VERSION"
if [ ! -d "$ANDROID_NDK" ]; then
  echo "ANDROID NDK version $ANDROID_NDK_VERSION is not installed"
  exit 1
fi

if [ "$COMPILE_ONLY" == "false" ]; then
device_status=$($ADB devices | grep "$IP" | awk '{print $2}')

  if [ -n "$device_status" ] && [ "$device_status" != "device" ]; then
    $ADB kill-server
  fi

  if [ -z "$device_status" ]; then
    $ADB connect $IP
    device_status=$($ADB devices | grep "$IP" | awk '{print $2}')
    if [ "$device_status" != "device" ]; then
      echo "Device $IP has an error: $device_status"
      exit 1
    fi
  fi
fi

# Sync the time between the development machine and the Android device
# Required later on for comparing timestamps of files
if [ "$COMPILE_ONLY" == "false" ]; then
  # Retry logic for time synchronization
  for attempt in {1..3}; do
    # Get the current time and set it on the Android device
    CURRENT_TIME=$(date +%m%d%H%M%Y.%S)
    $ADB -s $IP shell "date $CURRENT_TIME" > /dev/null 2>&1

    # Get the time on the development machine and Android device
    DEV_TIME=$(date +%s)
    ANDROID_TIME=$($ADB -s $IP shell "date +%s")

    # Check if the times are within 1 second of each other
    if [ $((DEV_TIME - ANDROID_TIME)) -le 1 ] && [ $((ANDROID_TIME - DEV_TIME)) -le 1 ]; then
      break
    fi

    if [ "$attempt" -eq 3 ]; then
      echo "Time on the development machine and Android device do not match within 1 second after 3 attempts."
      echo "Development machine time: $DEV_TIME"
      echo "Android device time: $ANDROID_TIME"
      exit 1
    fi

    echo "Time mismatch detected. Retrying... (Attempt $attempt)"
    sleep 0.1
  done
fi

# Check if lib_loader was updated
LIBLOADER_SRC="$SCRIPT_DIR/sdk/lib_loader"
LIBLOADER_SO_REMOTE="$TARGET_DIR/liblib_loader.so"

if [ "$COMPILE_ONLY" == "false" ]; then
  # Check if lib_loader source directory exists and is not empty
  if [ ! -d "$LIBLOADER_SRC" ]; then
    echo "lib_loader source directory does not exist: $LIBLOADER_SRC"
    exit 1
  elif [ -z "$(ls -A "$LIBLOADER_SRC")" ]; then
    echo "lib_loader source directory is empty: $LIBLOADER_SRC"
    exit 1
  fi

  # Check if lib_loader.so exists on the device
  # If not set the timestamp to 0
  LIBLOADER_SO_TIMESTAMP=$($ADB -s $IP shell stat -c %Y "$LIBLOADER_SO_REMOTE" 2>/dev/null || echo 0)
  LIBLOADER_SRC_TIMESTAMP=$(find "$LIBLOADER_SRC" -type f -exec stat -c %Y {} + | sort -n | tail -1)

  if [ "$LIBLOADER_SRC_TIMESTAMP" -gt "$LIBLOADER_SO_TIMESTAMP" ]; then
    echo "lib_loader was updated, restarting the app"
    $ADB -s $IP shell am force-stop com.qualcomm.ftcrobotcontroller
  fi
fi

mkdir -p $LIB_DIR

# Check if CMake directory exists
CMAKE_BASE_DIR="$ANDROID_SDK/cmake"
if [ ! -d "$CMAKE_BASE_DIR" ]; then
  echo "CMake is not  installed at the expected location: $CMAKE_BASE_DIR"
  exit 1
fi

# Check if specific CMake version exists
CMAKE_FOLDER="$CMAKE_BASE_DIR/$CMAKE_VERSION/bin"
if [ ! -d "$CMAKE_FOLDER" ]; then
  echo "CMake version $CMAKE_VERSION is not installed at the expected location: $CMAKE_FOLDER"
  exit 1
fi

# Check if cmake executable exists
if [ ! -x "$CMAKE_FOLDER/cmake" ]; then
  echo "cmake not found or not executable in: $CMAKE_FOLDER"
  exit 1
fi

# Check if ninja executable exists
if [ "$machine" == "Windows" ]; then
  if [ ! -x "$CMAKE_FOLDER/ninja.exe" ]; then
    echo "ninja.exe not found or not executable in: $CMAKE_FOLDER"
    exit 1
  fi
else
  if [ ! -x "$CMAKE_FOLDER/ninja" ]; then
    echo "ninja not found or not executable in: $CMAKE_FOLDER"
    exit 1
  fi
fi

if [ "$machine" == "Windows" ]; then
  $CMAKE_FOLDER/cmake -G "Ninja" -DCMAKE_MAKE_PROGRAM="$CMAKE_FOLDER/ninja.exe" -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK/build/cmake/android.toolchain.cmake -DANDROID_ABI=arm64-v8a -DANDROID_NATIVE_API_LEVEL=21 -DCMAKE_BUILD_TYPE=Debug -S "$SCRIPT_DIR" -B "$LIB_DIR"
else
  # Alternatively, Unix Makefiles can be used for better error output, although Ninja is faster
  # $CMAKE_FOLDER/cmake -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK/build/cmake/android.toolchain.cmake -DANDROID_ABI=arm64-v8a -DANDROID_NATIVE_API_LEVEL=21 -DCMAKE_BUILD_TYPE=Debug -S "$SCRIPT_DIR" -B "$LIB_DIR"
  $CMAKE_FOLDER/cmake -G "Ninja" -DCMAKE_MAKE_PROGRAM="$CMAKE_FOLDER/ninja" -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK/build/cmake/android.toolchain.cmake -DANDROID_ABI=arm64-v8a -DANDROID_NATIVE_API_LEVEL=21 -DCMAKE_BUILD_TYPE=Debug -S "$SCRIPT_DIR" -B "$LIB_DIR"
fi
$CMAKE_FOLDER/cmake --build "$LIB_DIR" -j4 --target all

if [ "$COMPILE_ONLY" == "true" ]; then
  exit 0
fi

for lib in "$LIB_DIR"/*.so; do
  # Check for updated libraries and push them
  if [ -f "$lib" ]; then
    local_timestamp=$(stat -c %Y "$lib")
    remote_timestamp=$($ADB -s $IP shell stat -c %Y "$TARGET_DIR/$(basename "$lib")" 2>/dev/null || echo 0)
    if [ "$local_timestamp" -gt "$remote_timestamp" ]; then
      for attempt in {1..3}; do
        $ADB -s $IP push "$lib" "$TARGET_DIR"

        # Check whether the libraries were pushed correctly
        lib_name=$(basename "$lib")
        remote_file_exists=$($ADB -s $IP shell "if [ -f $TARGET_DIR/$lib_name ]; then echo 'exists'; else echo 'not found'; fi")
        
        if [ "$remote_file_exists" == "exists" ]; then
          echo "$lib_name was transferred successfully."
          break
        else
          echo "Error transferring $lib_name. Retrying... (Attempt $attempt)"
          if [ "$attempt" -eq 3 ]; then
            echo "$lib_name was not pushed correctly after 3 attempts."
            exit 1
          fi
          sleep 0.1
        fi
      done
    fi
  fi
done