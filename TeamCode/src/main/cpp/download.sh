#!/bin/bash

# Detect OS
OS="$(uname -s)"
case "$OS" in
    Linux*)     machine=Linux;;
    Darwin*)    machine=Mac;;
    CYGWIN*|MINGW*|MSYS*) machine=Windows;;
    *)          machine="UNKNOWN:${OS}"
esac

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

if [ -z "$ANDROID_SDK" ]; then
    echo "Android SDK not found!"
    exit 1
fi

device_status=$($ADB devices | grep "192.168.43.1:5555" | awk '{print $2}')

if [ -n "$device_status" ] && [ "$device_status" != "device" ]; then
  $ADB kill-server
fi

if [ -z "$device_status" ]; then
  $ADB disconnect &&
  $ADB connect 192.168.43.1:5555
  device_status=$($ADB devices | grep "192.168.43.1:5555" | awk '{print $2}')
  if [ "$device_status" != "device" ]; then
    echo "Device 192.168.43.1:5555 has an error: $device_status"
    exit 1
  fi
fi

$ADB shell am force-stop com.qualcomm.ftcrobotcontroller &&

# Get the directory of the current script
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

ANDROID_NDK=$ANDROID_SDK/ndk/21.4.7075529/

LIB_DIR="$SCRIPT_DIR/../../../../build"
TARGET_DIR="/data/local/tmp"

mkdir -p $LIB_DIR

# Store timestamps of existing libraries
declare -A lib_timestamps
for lib in "$LIB_DIR"/*.so; do
  if [ -f "$lib" ]; then
    lib_timestamps["$lib"]=$(stat -c %Y "$lib")
  fi
done

CMAKE_FOLDER="$ANDROID_SDK/cmake/3.18.1/bin"

if [ "$machine" == "Windows" ]; then
  $CMAKE_FOLDER/cmake -G "Ninja" -DCMAKE_MAKE_PROGRAM="$CMAKE_FOLDER/ninja.exe" -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK/build/cmake/android.toolchain.cmake -DANDROID_ABI=arm64-v8a -DANDROID_NATIVE_API_LEVEL=21 -DCMAKE_BUILD_TYPE=Debug -S "$SCRIPT_DIR" -B "$LIB_DIR"
else
  # Alternatively, Unix Makefiles can be used for better error output, although Ninja is faster
  # $CMAKE_FOLDER/cmake -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK/build/cmake/android.toolchain.cmake -DANDROID_ABI=arm64-v8a -DANDROID_NATIVE_API_LEVEL=21 -DCMAKE_BUILD_TYPE=Debug -S "$SCRIPT_DIR" -B "$LIB_DIR"
  $CMAKE_FOLDER/cmake -G "Ninja" -DCMAKE_MAKE_PROGRAM="$CMAKE_FOLDER/ninja" -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK/build/cmake/android.toolchain.cmake -DANDROID_ABI=arm64-v8a -DANDROID_NATIVE_API_LEVEL=21 -DCMAKE_BUILD_TYPE=Debug -S "$SCRIPT_DIR" -B "$LIB_DIR"
fi
$CMAKE_FOLDER/cmake --build "$LIB_DIR" --target all &&

# Check for updated libraries and push them
for lib in "$LIB_DIR"/*.so; do
  if [ -f "$lib" ]; then
    new_timestamp=$(stat -c %Y "$lib")
    if [ "${lib_timestamps["$lib"]}" != "$new_timestamp" ]; then
      $ADB push "$lib" "$TARGET_DIR"
    fi
  fi
done

#adb shell chmod 755 $TARGET_DIR/libteamcode.so &&
#adb shell chmod 755 $TARGET_DIR/libsdk.so &&
#adb shell chmod 755 $TARGET_DIR/liblib_loader.so