cmake_minimum_required (VERSION 3.10)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project(hram VERSION 0.1 LANGUAGES C CXX)

include(FetchContent)

# vars

configure_file(vars.h.in genvars/vars.h)

# exe

add_executable (hram
	# WIN32
	MACOSX_BUNDLE

	"src/main.cpp"
	
 )

# version vars

target_include_directories(hram PUBLIC "${PROJECT_BINARY_DIR}/genvars")

# sdl

set(SDL_STATIC ON)
FetchContent_Declare(
    sdl3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL
    GIT_TAG release-3.2.16
)
FetchContent_MakeAvailable(sdl3)
target_link_libraries(hram PRIVATE SDL3::SDL3-static)
target_include_directories(hram PUBLIC SDL3::Headers)

# wasmtime

FetchContent_Declare(
    wasmtime
    GIT_REPOSITORY https://github.com/bytecodealliance/wasmtime.git
    SOURCE_SUBDIR crates/c-api
    OVERRIDE_FIND_PACKAGE
    SYSTEM
    GIT_TAG v34.0.1
)
FetchContent_MakeAvailable(wasmtime)
target_link_libraries(hram PUBLIC wasmtime)
target_include_directories(hram PUBLIC wasmtime)

# c / cxx standards

set_property(TARGET hram PROPERTY C_STANDARD 23)
set_property(TARGET hram PROPERTY CXX_STANDARD 26)
# /NODEFAULTLIB:library

# install

install(TARGETS hram DESTINATION .)
install(FILES $<TARGET_FILE:SDL3::SDL3> DESTINATION .)

# cpack

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set(CPACK_GENERATOR "ZIP")
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
include(CPack)
