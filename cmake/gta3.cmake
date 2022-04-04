######################################################################
################################ GTA 3 ###############################
set(CHAOS_3 ${PROJECT_NAME}.III)

file(GLOB_RECURSE III_SOURCES CONFIGURE_DEPENDS src/ChaosModComplementaries.cpp src/shared/*.cpp src/gta3/*.cpp)

add_library(${CHAOS_3} SHARED ${III_SOURCES})

set_target_properties(${CHAOS_3} PROPERTIES SUFFIX ".asi")

target_include_directories(${CHAOS_3} PUBLIC "src/shared/" "src/gta3")

target_link_libraries(${CHAOS_3} PUBLIC plugin_III)

target_compile_definitions(${CHAOS_3} PUBLIC NOMINMAX)
######################################################################
