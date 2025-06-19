# CMakeList.txt : CMake project for hram, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("hram")

# Add source to this project's executable.
add_executable (hram "hram.cpp" )

find_package(fmt CONFIG REQUIRED)
target_link_libraries(hram PRIVATE fmt::fmt)

find_package(SDL3 CONFIG REQUIRED)
target_link_libraries(hram PRIVATE SDL3::SDL3)

find_package(wabt CONFIG REQUIRED)
target_link_libraries(hram PRIVATE wabt::wabt)

if (CMAKE_VERSION VERSION_GREATER 3.31)
  set_property(TARGET hram PROPERTY CXX_STANDARD 26)
endif()

# TODO: Add tests and install targets if needed.
