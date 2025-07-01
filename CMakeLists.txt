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

configure_file(vars.h.in vars.h)
configure_file(boot.wat boot.wat COPYONLY)

add_executable (hram "src/main.cpp"  )

#set_property(TARGET hram PROPERTY WIN32_EXECUTABLE TRUE)

target_include_directories(hram PUBLIC ${PROJECT_BINARY_DIR})

find_package(SDL3 CONFIG REQUIRED)
target_link_libraries(hram PRIVATE SDL3::SDL3)

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


set_property(TARGET hram PROPERTY C_STANDARD 23)
set_property(TARGET hram PROPERTY CXX_STANDARD 26)

# TODO: Add tests and install targets if needed.

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set(CPACK_GENERATOR "TGZ")
set(CPACK_SOURCE_GENERATOR "TGZ")
include(CPack)
