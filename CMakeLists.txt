cmake_minimum_required (VERSION 3.10)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project(hram VERSION 0.1 LANGUAGES C CXX)

include(FetchContent)
include(ExternalProject)

# vars

configure_file(vars.h.in genvars/vars.h)

# exe

add_executable (hram
	# WIN32
	MACOSX_BUNDLE

    "lua-5.4.8/lauxlib.c"
    "lua-5.4.8/lbaselib.c"
    "lua-5.4.8/lcode.c"
    "lua-5.4.8/lcorolib.c"
    "lua-5.4.8/lctype.c"
    "lua-5.4.8/ldblib.c"
    "lua-5.4.8/ldebug.c"
    "lua-5.4.8/ldo.c"
    "lua-5.4.8/ldump.c"
    "lua-5.4.8/lfunc.c"
    "lua-5.4.8/lgc.c"
    "lua-5.4.8/linit.c"
    "lua-5.4.8/liolib.c"
    "lua-5.4.8/llex.c"
    "lua-5.4.8/lmathlib.c"
    "lua-5.4.8/lmem.c"
    "lua-5.4.8/loadlib.c"
    "lua-5.4.8/lobject.c"
    "lua-5.4.8/lopcodes.c"
    "lua-5.4.8/loslib.c"
    "lua-5.4.8/lparser.c"
    "lua-5.4.8/lstate.c"
    "lua-5.4.8/lstring.c"
    "lua-5.4.8/lstrlib.c"
    "lua-5.4.8/ltable.c"
    "lua-5.4.8/ltablib.c"
    "lua-5.4.8/ltm.c"
    "lua-5.4.8/lundump.c"
    "lua-5.4.8/lutf8lib.c"
    "lua-5.4.8/lvm.c"
    "lua-5.4.8/lzio.c"
    "lua-5.4.8/lapi.c"

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
