# This cmake file is adapted from:
# https://www.llvm.org/docs/CMake.html#embedding-llvm-in-your-project

# This file assumes llvm-config.exe is on the system 
# PATH, pointing to a valid llvm build somewhere on the
# system.

cmake_minimum_required(VERSION 3.13.4)
project(Iter0)

################
# Project Code #
################

add_executable(Iter0 main.cpp)

################################
# C++ Compiler Arguments/Flags #
################################

# TODO: I think most of these flags are not 
# useful here, it might only be the include folders
# that are needed.

execute_process(COMMAND llvm-config --cxxflags OUTPUT_VARIABLE CXX_FLAGS)
separate_arguments(CXX_FLAGS NATIVE_COMMAND ${CXX_FLAGS})
list(REMOVE_ITEM CXX_FLAGS "/GR-" "/EHs-c-")
message("\nC++ Flags: ${CXX_FLAGS}\n")
add_definitions(${CXX_FLAGS})

# TODO: What doe sthis do again lol?
add_compile_options("-Wargument")

####################
# Linked Libraries #
####################

execute_process(COMMAND llvm-config --libs support core passes irreader orcjit native OUTPUT_VARIABLE LLVM_LIBRARIES)
string(STRIP ${LLVM_LIBRARIES} LLVM_LIBRARIES)
string(REPLACE "\\" "/" LLVM_LIBRARIES ${LLVM_LIBRARIES})
separate_arguments(LLVM_LIBRARIES)
message(STATUS "\nFound libraries: ${LLVM_LIBRARIES}\n\n")
target_link_libraries(Iter0 ${LLVM_LIBRARIES})
