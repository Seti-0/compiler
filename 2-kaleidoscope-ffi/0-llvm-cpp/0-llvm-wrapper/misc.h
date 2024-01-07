#ifndef LLVM_WRAPPER_MISC_H
#define LLVM_WRAPPER_MISC_H

#define API_FUNCTION extern "C" __declspec(dllexport)

API_FUNCTION int LW_Getch();
API_FUNCTION short LW_GetKeyState(int virtual_key);
API_FUNCTION void LW_CopyToClipboard(char* text);

#endif