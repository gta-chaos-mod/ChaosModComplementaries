cmake_minimum_required(VERSION 3.13)
project(ChaosModComplementaries)

# C++20/17
set (CMAKE_CXX_STANDARD 20)

######################################################################
########################## Fetch plugin-sdk ##########################
include(FetchContent)
FetchContent_Declare(plugin-sdk
  GIT_REPOSITORY https://github.com/gta-chaos-mod/plugin-sdk
  GIT_SHALLOW)

set(PSDK_BUILD_III ON)
set(PSDK_BUILD_VC ON)
set(PSDK_BUILD_SA ON)

FetchContent_MakeAvailable(plugin-sdk)
######################################################################

######################################################################
########################### Include CMakes ###########################
include(cmake/gta3.cmake)
include(cmake/gtaVC.cmake)
include(cmake/gtaSA.cmake)
######################################################################
