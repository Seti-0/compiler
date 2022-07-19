# This cmake file is adapted from: 
# https://www.llvm.org/docs/CMake.html#embedding-llvm-in-your-project

cmake_minimum_required(VERSION 3.13.4)
project(Kaleidoscope)

################
# Project Code #
################

add_executable(Kaleidoscope main.cpp)

################################
# C++ Compiler Arguments/Flags #
################################

# I'm not sure this section is needed or even helpful.
# TODO: Look through the flags one by one and see if any make sense to apply here.

execute_process(COMMAND llvm-config --cxxflags OUTPUT_VARIABLE CXX_FLAGS)
separate_arguments(CXX_FLAGS NATIVE_COMMAND ${CXX_FLAGS})
list(REMOVE_ITEM CXX_FLAGS "/GR-" "/EHs-c-")
message("\nC++ Flags: ${CXX_FLAGS}\n")
add_definitions(${CXX_FLAGS})

add_compile_options("-Wargument")

####################
# Linked Libraries #
####################

execute_process(COMMAND llvm-config --libs support core passes irreader orcjit native OUTPUT_VARIABLE LLVM_LIBRARIES)
string(STRIP ${LLVM_LIBRARIES} LLVM_LIBRARIES)
string(REPLACE "\\" "/" LLVM_LIBRARIES ${LLVM_LIBRARIES})
separate_arguments(LLVM_LIBRARIES)
target_link_libraries(Kaleidoscope ${LLVM_LIBRARIES})
message(STATUS "\nFound libraries: ${LLVM_LIBRARIES}\n\n")

#########
# Notes #
#########

# This file assumes that there is llvm-config.exe somewhere on the system path.
# It is there that it gets information on include file locations, linked library locations,
# and c++ compiler flags.

# The output from llvm config is space separated, the "separate_arguments" changes this to
# semi-colon separation. semi-colon separated values are lists in cmake.

# llvm-config yields backslash based paths, cmake expects forward-slash based ones.
