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
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS})

# Now build our tools
add_executable(Kaleidoscope main.cpp)

# Find the libraries that correspond to the LLVM components
# that we wish to use
#llvm_map_components_to_libnames(llvm_libs support core passes irreader)

#execute_process(COMMAND llvm-config --libs support core passes irreader OUTPUT_VARIABLE LLVM_LIBRARIES)
#string(STRIP ${LLVM_LIBRARIES} LLVM_LIBRARIES)
#message(STATUS "Found LLVM libraries: ${LLVM_LIBRARIES}")
#message(STATUS "Found LLVM libs: ${llvm_libs}")

# Link against LLVM libraries
# I should really figure out a way to not hardcode this!
target_link_libraries(Kaleidoscope "C:/Users/Kieran/Desktop/llvm-build/Debug/lib/LLVMIRReader.lib;C:/Users/Kieran/Desktop/llvm-build/Debug/lib/LLVMBitReader.lib;C:/Users/Kieran/Desktop/llvm-build/Debug/lib/LLVMAsmParser.lib;C:/Users/Kieran/Desktop/llvm-build/Debug/lib/LLVMCore.lib;C:/Users/Kieran/Desktop/llvm-build/Debug/lib/LLVMRemarks.lib;C:/Users/Kieran/Desktop/llvm-build/Debug/lib/LLVMBitstreamReader.lib;C:/Users/Kieran/Desktop/llvm-build/Debug/lib/LLVMBinaryFormat.lib;C:/Users/Kieran/Desktop/llvm-build/Debug/lib/LLVMSupport.lib;C:/Users/Kieran/Desktop/llvm-build/Debug/lib/LLVMDemangle.lib;psapi.lib;shell32.lib;ole32.lib;uuid.lib;advapi32.lib")
#target_link_libraries(Kaleidoscope ${llvm_libs})
#target_link_libraries(Kaleidoscope ${LLVM_LIBRARIES})

# Suppress some warnings
#  - 4244: numeric conversions that can cause data loss. There are a ton of these 
#          that are really trivial and not worth seeing.
#  - 4199: unused, and thus ignored, dlls. The fact that these are ignored seems fine,
#          and I don't know why they are there.
# This does not actually work though!!!
# set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/ignore:4244,4199")
