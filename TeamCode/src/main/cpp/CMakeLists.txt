# Sets the minimum version of CMake required to build your native library.
# This ensures that a certain set of CMake features is available to
# your build.

cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project("${CMAKE_PROJECT_NAME}")

# Path to the directory containing source files
set(TEAMCODE_SOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/teamcode/sources)
# image recognition
set(IMAGE_RECOGNITION_SOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/teamcode/imageRec/src)
set(SDK_SOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/sdk/sources)

# Collect all source files in the directory and its subdirectories
file(GLOB_RECURSE TEAMCODE_SOURCES CONFIGURE_DEPENDS ${TEAMCODE_SOURCES_DIR}/*.cpp)
# image recognition
file(GLOB_RECURSE IMAGE_RECOGNITION_SOURCES CONFIGURE_DEPENDS ${IMAGE_RECOGNITION_SOURCES_DIR}/*.cpp)
file(GLOB_RECURSE SDK_SOURCES CONFIGURE_DEPENDS ${SDK_SOURCES_DIR}/*.cpp)

# Update the existing files in the build
add_library(teamcode SHARED ${TEAMCODE_SOURCES} ${IMAGE_RECOGNITION_SOURCES})
add_library(sdk SHARED ${SDK_SOURCES})

# Separate library for lib_loader.cpp
add_library(lib_loader SHARED ${CMAKE_CURRENT_SOURCE_DIR}/sdk/lib_loader/lib_loader.cpp)

# For armeabi-v7a
add_library(april_tag_v7a SHARED IMPORTED)
set_target_properties(april_tag_v7a PROPERTIES IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/../../../build/intermediates/merged_native_libs/debug/out/lib/armeabi-v7a/libapriltag.so")

add_library(opencv_java4_v7a SHARED IMPORTED)
set_target_properties(opencv_java4_v7a PROPERTIES IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/../../../build/intermediates/merged_native_libs/debug/out/lib/armeabi-v7a/libopencv_java4.so")

# For arm64-v8a
add_library(april_tag_v8a SHARED IMPORTED)
set_target_properties(april_tag_v8a PROPERTIES IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/../../../build/intermediates/merged_native_libs/debug/out/lib/arm64-v8a/libapriltag.so")

add_library(opencv_java4_v8a SHARED IMPORTED)
set_target_properties(opencv_java4_v8a PROPERTIES IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/../../../build/intermediates/merged_native_libs/debug/out/lib/arm64-v8a/libopencv_java4.so")

# Set the exact path to the log library
set(log-lib ${CMAKE_ANDROID_NDK}/platforms/android-21/arch-arm64/usr/lib/liblog.so)

# Link the libraries
target_link_libraries(
        sdk
        ${log-lib}
        $<$<STREQUAL:${ANDROID_ABI},armeabi-v7a>:april_tag_v7a>
        $<$<STREQUAL:${ANDROID_ABI},armeabi-v7a>:opencv_java4_v7a>
        $<$<STREQUAL:${ANDROID_ABI},arm64-v8a>:april_tag_v8a>
        $<$<STREQUAL:${ANDROID_ABI},arm64-v8a>:opencv_java4_v8a>
)

target_link_libraries(
        lib_loader
        ${log-lib}
)

target_link_libraries(
        teamcode
        sdk
)

# Specifies a path to native header files.
include_directories(
        teamcode/header
        sdk/header
        external_libraries
        sdk/lib_loader/lib_loader.h
)