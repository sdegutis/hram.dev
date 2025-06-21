# CMakeList.txt : CMake project for hram, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.10)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project(hram VERSION 0.1 LANGUAGES C CXX)

include(FetchContent)

configure_file(config.h.in config.h)

add_executable (hram "src/hram.cpp"  "src/wat.cpp")

#set_property(TARGET hram PROPERTY WIN32_EXECUTABLE TRUE)

target_include_directories(hram PUBLIC ${PROJECT_BINARY_DIR})

find_package(SDL3 CONFIG REQUIRED)
target_link_libraries(hram PRIVATE SDL3::SDL3)

find_package(wabt CONFIG REQUIRED)
target_link_libraries(hram PRIVATE wabt::wabt)

set (WAMR_BUILD_PLATFORM "windows")
set (WAMR_BUILD_TARGET "X86_64")

set (WAMR_BUILD_LIBC_BUILTIN 0)
set (WAMR_BUILD_LIBC_WASI 0)
set (WAMR_BUILD_LIBC_UVWASI 0)

set (WAMR_BUILD_INTERP 1)
set (WAMR_BUILD_FAST_INTERP 1)

set (WAMR_BUILD_AOT 1)
set (WAMR_BUILD_JIT 0)

set (WAMR_BUILD_SIMD 0)

FetchContent_Declare(
    wamr
    GIT_REPOSITORY https://github.com/bytecodealliance/wasm-micro-runtime.git
    GIT_TAG WAMR-2.3.1
)
FetchContent_Populate(wamr)
include (${wamr_SOURCE_DIR}/build-scripts/runtime_lib.cmake)
add_library(vmlib ${WAMR_RUNTIME_LIB_SOURCE})
target_link_libraries (hram PRIVATE vmlib -lm)


if (CMAKE_VERSION VERSION_GREATER 3.31)
  set_property(TARGET hram PROPERTY CXX_STANDARD 26)
endif()

# TODO: Add tests and install targets if needed.

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set(CPACK_GENERATOR "TGZ")
set(CPACK_SOURCE_GENERATOR "TGZ")
include(CPack)
