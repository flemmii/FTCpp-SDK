cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(SoftwareTests)

# Google Test hinzufügen
set(GTEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../external_libraries/gtest)

include_directories(${GTEST_DIR}/include)
link_directories(${GTEST_DIR}/lib)

# Testdateien hinzufügen
file(GLOB TEST_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

# Task Scheduler-Quellcode hinzufügen
set(TASK_SCHEDULER_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/../sources/task_scheduler.cpp
)

# Test-Ziel erstellen
add_executable(task_scheduler_test ${TEST_SOURCES} ${TASK_SCHEDULER_SOURCES})

# Abhängigkeiten hinzufügen
target_include_directories(task_scheduler_test PRIVATE ../header ../../sdk/header ../../external_libraries)
target_link_libraries(task_scheduler_test gtest gtest_main pthread)

# Disable warnings for OpenCV2 headers in tests
target_include_directories(task_scheduler_test SYSTEM PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../external_libraries/opencv2)

# Define SOFTWARE_TEST_MODE for tests
add_compile_definitions(SOFTWARE_TEST_MODE)

# Tests automatisch ausführen
enable_testing()
add_test(NAME task_scheduler_test COMMAND task_scheduler_test)