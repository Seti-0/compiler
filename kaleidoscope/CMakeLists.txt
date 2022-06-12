# This cmake file is adapted from: 
# https://www.llvm.org/docs/CMake.html#embedding-llvm-in-your-project

cmake_minimum_required(VERSION 3.13.4)
project(Kaleidoscope)

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# Set your project compile flags.
# E.g. if using the C++ header files
# you will need to enable C++11 support
# for your compiler.

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# Now build our tools
add_executable(Kaleidoscope main.cpp)

# Find the libraries that correspond to the LLVM components
# that we wish to use
llvm_map_components_to_libnames(llvm_libs support core irreader)

# Link against LLVM libraries
target_link_libraries(Kaleidoscope ${llvm_libs})

# Suppress some warnings
# Might be worth looking at these again, but they are pretty insignificant.
#  - 4244: numeric conversions that can cause data loss. There are a ton of these 
#          that are really trivial and not worth seeing.
#  - 4199: unused, and thus ignored, dlls. The fact that these are ignored seems fine,
#          and I don't know why they are there.
set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/ignore:4244,4199")
