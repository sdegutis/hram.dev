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

	"lua55/lbaselib.c"
	"lua55/lcode.c"
	"lua55/lcorolib.c"
	"lua55/lctype.c"
	"lua55/ldblib.c"
	"lua55/ldebug.c"
	"lua55/ldo.c"
	"lua55/ldump.c"
	"lua55/lfunc.c"
	"lua55/lgc.c"
	"lua55/linit.c"
	"lua55/liolib.c"
	"lua55/llex.c"
	"lua55/lmathlib.c"
	"lua55/lmem.c"
	"lua55/loadlib.c"
	"lua55/lobject.c"
	"lua55/lopcodes.c"
	"lua55/loslib.c"
	"lua55/lparser.c"
	"lua55/lstate.c"
	"lua55/lstring.c"
	"lua55/lstrlib.c"
	"lua55/ltable.c"
	"lua55/ltablib.c"
	"lua55/ltm.c"
	"lua55/lundump.c"
	"lua55/lutf8lib.c"
	"lua55/lvm.c"
	"lua55/lzio.c"
	"lua55/lapi.c"
	"lua55/lauxlib.c"

	"src/main.cpp"
	
 )

# version vars

target_include_directories(hram PUBLIC "${PROJECT_SOURCE_DIR}/lua55")
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

# install

install(TARGETS hram DESTINATION .)
install(FILES $<TARGET_FILE:SDL3::SDL3> DESTINATION .)
install(FILES "${PROJECT_BINARY_DIR}/boot.lua" DESTINATION .)
install(FILES "${PROJECT_BINARY_DIR}/editor.lua" DESTINATION .)
install(FILES "${PROJECT_BINARY_DIR}/prompt.lua" DESTINATION .)

# cpack

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set(CPACK_GENERATOR "ZIP")
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
include(CPack)
