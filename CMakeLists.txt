cmake_minimum_required(VERSION 3.16)
project(ChaosModComplementaries)

# C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POLICY_VERSION_MINIMUM 3.11)

if(MSVC_VERSION GREATER_EQUAL "1900")
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
  add_compile_options(/Zc:preprocessor /Zm500)
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

find_package(PluginSDK REQUIRED COMPONENTS gtasa)

# Include cmakes
if (PluginSDK_gta3_FOUND)
  include(cmake/gta3.cmake)
endif()

if (PluginSDK_gtavc_FOUND)
  include(cmake/gtaVC.cmake)
endif()

if (PluginSDK_gtasa_FOUND)
  include(cmake/gtaSA.cmake)
endif()

include(cmake/minhook.cmake)
include(cmake/efsw.cmake)
