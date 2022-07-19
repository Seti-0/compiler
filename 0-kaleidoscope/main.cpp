#pragma once

// There is a C4244 warning that pops up that looks loki some interaction between visual c++
// and LLVM. I don't know why it happens, or where I can disable it specifically.
#pragma warning( disable : 4244 )

#include <string>
#include <iostream>
#include <vector>
#include <memory>
#include <map>

//#include "compiler/tokens.cpp"
#include "compiler/jit.cpp"

// ######################
// # NO LONGER RELEVANT #
// ######################
// (I still think it's worth keeping around though, it was so obscure)
//
// With an older version of the visual c++ toolset, there was a mistmatch between header files and
// a compiled library, as I understand it. These preproc commands link the old name to the new name
// for the sake of the linker. Without it, the linker can't resolve the old symbols.
//
//#if _MSC_VER >= 1932 // Visual Studio 2022 version 17.2+
//#    pragma comment(linker, "/alternatename:__imp___std_init_once_complete=__imp_InitOnceComplete")
//#    pragma comment(linker, "/alternatename:__imp___std_init_once_begin_initialize=__imp_InitOnceBeginInitialize")
//#endif
//
// This is no longer relevant though, since it seems to be fixed in newer version of the c++ toolset.
// (Note - it's LLVM that needs to be built with the new toolset, so if I have to revert that for whatever reason
// this becomes a problem again.)
//
// More info: https://github.com/conan-io/conan-center-index/issues/4826
// Also: https://developercommunity.visualstudio.com/t/-imp-std-init-once-complete-unresolved-external-sy/1684365#T-N10041864
// Also: https://developercommunity.visualstudio.com/t/-imp-std-init-once-complete-unresolved-external-sy/1684365#T-N10047968


// TODO: Refactor tokens and expressions, maybe move from globals to args & returns.

int main() {
    jit::debug = true;
    jit::init();
    builtins::init();

    printf("V3\n");
    jit::interactive();
    
    jit::cleanup();
}
