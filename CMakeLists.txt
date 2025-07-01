cmake_minimum_required (VERSION 3.10)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project(hram VERSION 0.1 LANGUAGES C CXX)

include(FetchContent)

# vars

configure_file(vars.h.in genvars/vars.h)
configure_file(lua/boot.lua boot.lua COPYONLY)
configure_file(lua/editor.lua editor.lua COPYONLY)
configure_file(lua/prompt.lua prompt.lua COPYONLY)

# exe

add_executable (hram
	# WIN32
	MACOSX_BUNDLE
	"src/main.c" )

# version vars

target_include_directories(hram PUBLIC "${PROJECT_BINARY_DIR}/genvars")

# sdl

find_package(SDL3 CONFIG REQUIRED)
target_link_libraries(hram PRIVATE SDL3::SDL3)

# lua

find_package(Lua REQUIRED)
target_include_directories(hram PRIVATE ${LUA_INCLUDE_DIR})
target_link_libraries(hram PRIVATE ${LUA_LIBRARIES})

# c / cxx standards

set_property(TARGET hram PROPERTY C_STANDARD 23)
set_property(TARGET hram PROPERTY CXX_STANDARD 26)

# install

install(TARGETS hram DESTINATION .)
install(FILES $<TARGET_FILE:SDL3::SDL3> DESTINATION .)
install(FILES $<TARGET_FILE:${LUA_LIBRARIES}> DESTINATION .)
install(FILES "${PROJECT_BINARY_DIR}/boot.lua" DESTINATION .)
install(FILES "${PROJECT_BINARY_DIR}/editor.lua" DESTINATION .)
install(FILES "${PROJECT_BINARY_DIR}/prompt.lua" DESTINATION .)

# cpack

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set(CPACK_GENERATOR "ZIP")
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
include(CPack)
