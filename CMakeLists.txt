# CMakeList.txt : CMake project for hram, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

include(FetchContent)

project ("hram")

# Add source to this project's executable.
add_executable (hram "hram.cpp" )

find_package(fmt CONFIG REQUIRED)
target_link_libraries(hram PRIVATE fmt::fmt)

find_package(SDL3 CONFIG REQUIRED)
target_link_libraries(hram PRIVATE SDL3::SDL3)

find_package(wabt CONFIG REQUIRED)
target_link_libraries(hram PRIVATE wabt::wabt)

set (WAMR_BUILD_PLATFORM "windows")
set (WAMR_BUILD_TARGET "X86_64")
#set (WAMR_BUILD_INTERP 1)
#set (WAMR_BUILD_FAST_INTERP 1)
set (WAMR_BUILD_AOT 1)
#set (WAMR_BUILD_LIBC_BUILTIN 1)
#set (WAMR_BUILD_LIBC_WASI 1)
set (WAMR_BUILD_SIMD 0)

FetchContent_Declare(
    wamr
    GIT_REPOSITORY https://github.com/bytecodealliance/wasm-micro-runtime.git
    GIT_TAG WAMR-2.3.1
)
#FetchContent_MakeAvailable(wamr)
set (WAMR_ROOT_DIR ${CMAKE_CURRENT_BINARY_DIR}/_deps/wamr-src)
include (${WAMR_ROOT_DIR}/build-scripts/runtime_lib.cmake)
add_library(vmlib ${WAMR_RUNTIME_LIB_SOURCE})
target_link_libraries (hram PRIVATE vmlib -lm)


if (CMAKE_VERSION VERSION_GREATER 3.31)
  set_property(TARGET hram PROPERTY CXX_STANDARD 26)
endif()

# TODO: Add tests and install targets if needed.
