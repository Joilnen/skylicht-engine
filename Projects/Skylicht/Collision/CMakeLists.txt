include_directories(
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/Collision/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Irrlicht/Include
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Irrlicht/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/ThirdParty/source/curl/include
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/System/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/Audio/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/Engine/Source
)

file(GLOB_RECURSE skylicht_collision 
	./Source/*.cpp 
	./Source/*.hpp 
	./Source/*.c 
	./Source/*.h)
setup_project_group("${skylicht_collision}" ${CMAKE_CURRENT_SOURCE_DIR})

add_library(Collision STATIC
	${skylicht_collision}
)

target_precompiled_header(Collision ./Source/pch.cpp ${skylicht_collision})

set_target_properties(Collision PROPERTIES VERSION ${SKYLICHT_VERSION})

if (BUILD_EMSCRIPTEN)
target_link_libraries(Collision Engine System)
elseif(MSVC)
target_link_libraries(Collision Engine System)
elseif(CYGWIN OR MINGW)
target_link_libraries(Collision Engine System)
endif()