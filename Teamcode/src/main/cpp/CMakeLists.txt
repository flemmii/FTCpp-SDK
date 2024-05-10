# Sets the minimum version of CMake required to build your native library.
# This ensures that a certain set of CMake features is available to
# your build.

cmake_minimum_required(VERSION 3.4.1)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Specifies a library name, specifies whether the library is STATIC or
# SHARED, and provides relative paths to the source code. You can
# define multiple libraries by adding multiple add_library() commands,
# and CMake builds them for you. When you build your app, Gradle
# automatically packages shared libraries with your APK.

project("${CMAKE_PROJECT_NAME}")

# Pfad zum Verzeichnis mit den Quellcodedateien
set(SOURCES_DIR teamcode/sources)
set(SDK_SOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/sdk/sources)

# Alle vorhandenen Quelldateien im Verzeichnis und seinen Unterordnern sammeln
file(GLOB_RECURSE teamcode/sources CONFIGURE_DEPENDS ${SOURCES_DIR}/*.cpp)
file(GLOB_RECURSE SDK_SOURCES CONFIGURE_DEPENDS ${SDK_SOURCES_DIR}/*.cpp)

# Aktualisiere die vorhandenen Dateien im Build
add_library(native-lib SHARED ${SOURCES} ${SDK_SOURCES})

# Specifies a path to prebuilt native libraries outside of your project of OpenCV
add_library(april_tag SHARED IMPORTED)
set_target_properties(april_tag PROPERTIES IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}../../../../build/intermediates/merged_native_libs/debug/out/lib/armeabi-v7a/libapriltag.so")

add_library(image_utils SHARED IMPORTED)
set_target_properties(image_utils PROPERTIES IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}../../../../build/intermediates/merged_native_libs/debug/out/lib/armeabi-v7a/libimage_utils.so")

add_library(object_tracking SHARED IMPORTED)
set_target_properties(object_tracking PROPERTIES IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}../../../../build/intermediates/merged_native_libs/debug/out/lib/armeabi-v7a/libobject_tracking.so")

add_library(opencv_java4 SHARED IMPORTED)
set_target_properties(opencv_java4 PROPERTIES IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}../../../../build/intermediates/merged_native_libs/debug/out/lib/armeabi-v7a/libopencv_java4.so")


find_library(log-lib log)
target_link_libraries(
        native-lib
        ${log-lib}
        april_tag
        image_utils
        object_tracking
        opencv_java4)

# Specifies a path to native header files.
include_directories(
        teamcode/header
        sdk/header
        external_libraries)

#ignore all geometrical drive files
set_directory_properties(PROPERTIES
        EXCLUDE_FROM_ALL "teamcode/sources/geometrical_drive.cpp;sources/geometrical_drive_grapher.cpp;header/geometrical_drive.h;header/geometrical_drive_grapher.h;"
)
