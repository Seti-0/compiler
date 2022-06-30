#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <memory>
#include <map>

#include "compiler/jit.cpp"

// As I understand it, there was an errant (unlisted) breaking change for visual c++ and a specific api.
// This tells the linker that there is a new name for the two functions changed.
// More info: https://github.com/conan-io/conan-center-index/issues/4826
// Also: https://developercommunity.visualstudio.com/t/-imp-std-init-once-complete-unresolved-external-sy/1684365#T-N10041864
// Also: https://developercommunity.visualstudio.com/t/-imp-std-init-once-complete-unresolved-external-sy/1684365#T-N10047968
// The more complete fix is apparently to rebuild the library (llvm, I think) with a newer 'toolset' 
// (i.e. when using CMake to build llvm, use the latest visual c++ and visual studio setup)
#if _MSC_VER >= 1932 // Visual Studio 2022 version 17.2+
#    pragma comment(linker, "/alternatename:__imp___std_init_once_complete=__imp_InitOnceComplete")
#    pragma comment(linker, "/alternatename:__imp___std_init_once_begin_initialize=__imp_InitOnceBeginInitialize")
#endif

// TODO: Fix tracing starting one level up from where it should.
// TODO: Msg.
// TODO: Refactor tokens and expressions, maybe move from globals to args & returns.
// TODO: Consider refactoring away from using iterators in jit::execute, it's quite error-prone the way it is.
// TODO: Fix output from extern functions when importing. Maybe regroup extern definition output?
// TODO: Fix for loop end condition going one too far.
// TODO: Fix two imports in the same block causing problems. e.g. import pre;import mandel

int main() {
    //tokens::debug = true;
    expr::debug = true;
    gen::debug = true;
    jit::debug = true;
    jit::init();
    builtins::init();

    printf("V3\n");
    jit::interactive();
    
    jit::cleanup();
}
