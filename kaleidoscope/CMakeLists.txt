# This cmake file is adapted from: 
# https://www.llvm.org/docs/CMake.html#embedding-llvm-in-your-project

cmake_minimum_required(VERSION 3.13.4)
project(Kaleidoscope)

########################
# Collecting LLVM info #
########################

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

message(STATUS  "\nLLVM Include Dirs: ${LLVM_INCLUDE_DIRS}")
message(STATUS "LLVM Definitions: ${LLVM_DEFINITIONS}")

execute_process(COMMAND llvm-config --libs support core passes irreader orcjit native OUTPUT_VARIABLE LLVM_LIBRARIES)
string(STRIP ${LLVM_LIBRARIES} LLVM_LIBRARIES)
string(REPLACE "\\" "/" LLVM_LIBRARIES ${LLVM_LIBRARIES})
separate_arguments(LLVM_LIBRARIES)

message(STATUS "\nFound libraries: ${LLVM_LIBRARIES}\n\n")

######################
# Applying LLVM info #
######################

include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS})

add_executable(Kaleidoscope main.cpp)
target_link_libraries(Kaleidoscope ${LLVM_LIBRARIES})

#########
# Notes #
#########

# On Windows, I used a local build of LLVM for this, and should  really redo it so 
# that I can document it properly. There is decent documentation online, but it is a little out of date.
#
# Some notes:
#
# - There are two sources of information above that need to be installed for this script to work.
#   -> First, CMake must be able to find the LLVM package, for the sake of include files and C++ flags.
#   -> Second, the 'llvm-config' command must be available so that functionality can be linked to libraries.
#
# - The output from llvm-config is a list of space-separated paths with backslashes. It needs to be
#   trimmed, and converted to a semi-colon-separated list, and the backslashes need to be replaced with
#   forward slashes, before it can be used in the 'target_link_libraries' function.
#
# R.e. components:
#
# For just the IR generation, I used: 'support core passes irreader'
# For IR & JIT: 'support core passes irreader orcjit native'

#############
# OLD SETUP #
#############

# I'll delete this stuff when I'm confident the new stuff works.

#string(STRIP ${LLVM_LIBRARIES} LLVM_LIBRARIES)
#message(STATUS "Found LLVM libraries: ${LLVM_LIBRARIES}")
#message(STATUS "Found LLVM libs: ${llvm_libs}")

# Current comps: support core passes irreader orcjit native

# Link against LLVM libraries
# I should really figure out a way to not hardcode this!
# target_link_libraries(Kaleidoscope "C:/Users/Kieran/Desktop/llvm-build/Debug/lib/LLVMX86TargetMCA.lib;C:/Users/Kieran/Desktop/llvm-build/Debug/lib/LLVMMCA.lib;C:/Users/Kieran/Desktop/llvm-build/Debug/lib/LLVMX86Disassembler.lib;C:/Users/Kieran/Desktop/llvm-build/Debug/lib/LLVMX86AsmParser.lib;C:/Users/Kieran/Desktop/llvm-build/Debug/lib/LLVMX86CodeGen.lib;C:/Users/Kieran/Desktop/llvm-build/Debug/lib/LLVMCFGuard.lib;C:/Users/Kieran/Desktop/llvm-build/Debug/lib/LLVMGlobalISel.lib;C:/Users/Kieran/Desktop/llvm-build/Debug/lib/LLVMX86Desc.lib;C:/Users/Kieran/Desktop/llvm-build/Debug/lib/LLVMX86Info.lib;C:/Users/Kieran/Desktop/llvm-build/Debug/lib/LLVMSelectionDAG.lib;C:/Users/Kieran/Desktop/llvm-build/Debug/lib/LLVMAsmPrinter.lib;C:/Users/Kieran/Desktop/llvm-build/Debug/lib/LLVMCodeGen.lib;C:/Users/Kieran/Desktop/llvm-build/Debug/lib/LLVMOrcJIT.lib;C:/Users/Kieran/Desktop/llvm-build/Debug/lib/LLVMMCDisassembler.lib;C:/Users/Kieran/Desktop/llvm-build/Debug/lib/LLVMJITLink.lib;C:/Users/Kieran/Desktop/llvm-build/Debug/lib/LLVMExecutionEngine.lib;C:/Users/Kieran/Desktop/llvm-build/Debug/lib/LLVMRuntimeDyld.lib;C:/Users/Kieran/Desktop/llvm-build/Debug/lib/LLVMOrcTargetProcess.lib;C:/Users/Kieran/Desktop/llvm-build/Debug/lib/LLVMOrcShared.lib;C:/Users/Kieran/Desktop/llvm-build/Debug/lib/LLVMPasses.lib;C:/Users/Kieran/Desktop/llvm-build/Debug/lib/LLVMTarget.lib;C:/Users/Kieran/Desktop/llvm-build/Debug/lib/LLVMObjCARCOpts.lib;C:/Users/Kieran/Desktop/llvm-build/Debug/lib/LLVMCoroutines.lib;C:/Users/Kieran/Desktop/llvm-build/Debug/lib/LLVMipo.lib;C:/Users/Kieran/Desktop/llvm-build/Debug/lib/LLVMInstrumentation.lib;C:/Users/Kieran/Desktop/llvm-build/Debug/lib/LLVMVectorize.lib;C:/Users/Kieran/Desktop/llvm-build/Debug/lib/LLVMLinker.lib;C:/Users/Kieran/Desktop/llvm-build/Debug/lib/LLVMIRReader.lib;C:/Users/Kieran/Desktop/llvm-build/Debug/lib/LLVMAsmParser.lib;C:/Users/Kieran/Desktop/llvm-build/Debug/lib/LLVMFrontendOpenMP.lib;C:/Users/Kieran/Desktop/llvm-build/Debug/lib/LLVMScalarOpts.lib;C:/Users/Kieran/Desktop/llvm-build/Debug/lib/LLVMInstCombine.lib;C:/Users/Kieran/Desktop/llvm-build/Debug/lib/LLVMBitWriter.lib;C:/Users/Kieran/Desktop/llvm-build/Debug/lib/LLVMAggressiveInstCombine.lib;C:/Users/Kieran/Desktop/llvm-build/Debug/lib/LLVMTransformUtils.lib;C:/Users/Kieran/Desktop/llvm-build/Debug/lib/LLVMAnalysis.lib;C:/Users/Kieran/Desktop/llvm-build/Debug/lib/LLVMProfileData.lib;C:/Users/Kieran/Desktop/llvm-build/Debug/lib/LLVMSymbolize.lib;C:/Users/Kieran/Desktop/llvm-build/Debug/lib/LLVMDebugInfoPDB.lib;C:/Users/Kieran/Desktop/llvm-build/Debug/lib/LLVMDebugInfoMSF.lib;C:/Users/Kieran/Desktop/llvm-build/Debug/lib/LLVMDebugInfoDWARF.lib;C:/Users/Kieran/Desktop/llvm-build/Debug/lib/LLVMObject.lib;C:/Users/Kieran/Desktop/llvm-build/Debug/lib/LLVMTextAPI.lib;C:/Users/Kieran/Desktop/llvm-build/Debug/lib/LLVMMCParser.lib;C:/Users/Kieran/Desktop/llvm-build/Debug/lib/LLVMMC.lib;C:/Users/Kieran/Desktop/llvm-build/Debug/lib/LLVMDebugInfoCodeView.lib;C:/Users/Kieran/Desktop/llvm-build/Debug/lib/LLVMBitReader.lib;C:/Users/Kieran/Desktop/llvm-build/Debug/lib/LLVMCore.lib;C:/Users/Kieran/Desktop/llvm-build/Debug/lib/LLVMRemarks.lib;C:/Users/Kieran/Desktop/llvm-build/Debug/lib/LLVMBitstreamReader.lib;C:/Users/Kieran/Desktop/llvm-build/Debug/lib/LLVMBinaryFormat.lib;C:/Users/Kieran/Desktop/llvm-build/Debug/lib/LLVMSupport.lib;C:/Users/Kieran/Desktop/llvm-build/Debug/lib/LLVMDemangle.lib")
#target_link_libraries(Kaleidoscope ${llvm_libs})


# Suppress some warnings
#  - 4244: numeric conversions that can cause data loss. There are a ton of these 
#          that are really trivial and not worth seeing.
#  - 4199: unused, and thus ignored, dlls. The fact that these are ignored seems fine,
#          and I don't know why they are there.
# This does not actually work though!!!
# set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/ignore:4244,4199")
