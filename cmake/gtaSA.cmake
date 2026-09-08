# #####################################################################
# ########################## GTA San Andreas ##########################
set(CHAOS_SA ${PROJECT_NAME}.SA)

file(GLOB_RECURSE SA_SOURCES CONFIGURE_DEPENDS
	src/ChaosModComplementaries.cpp
	src/shared/*.cpp
	src/shared/*.h
	src/shared/*.hpp
	src/gtasa/*.cpp
	src/gtasa/*.h
	src/gtasa/*.hpp
)

add_library(${CHAOS_SA} SHARED ${SA_SOURCES})

set_target_properties(${CHAOS_SA} PROPERTIES SUFFIX ".asi")

target_include_directories(${CHAOS_SA} PUBLIC
	"src/"
	"src/shared"
	"src/gtasa"
)

target_link_libraries(${CHAOS_SA} PUBLIC PluginSDK::gtasa minhook efsw)

target_compile_definitions(${CHAOS_SA} PUBLIC NOMINMAX)

# #####################################################################