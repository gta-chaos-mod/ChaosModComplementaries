cmake_minimum_required(VERSION 3.16)
project(ChaosModComplementaries)

# C++20/17
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC_VERSION GREATER_EQUAL "1900")
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
  add_compile_options(/Zc:preprocessor /Zm500)
endif()

# Include cmakes
include(cmake/pluginSDK.cmake)

include(cmake/gta3.cmake)
include(cmake/gtaVC.cmake)
include(cmake/gtaSA.cmake)

include(cmake/minhook.cmake)
include(cmake/efsw.cmake)
