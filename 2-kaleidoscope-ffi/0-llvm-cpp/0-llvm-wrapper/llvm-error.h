#ifndef LLVM_WRAPPER_LLVM_ERROR_H
#define LLVM_WRAPPER_LLVM_ERROR_H

enum LW_ErrorCode {
    LW_ERROR_UNKNOWN
};

struct LW_Error
{
    LW_ErrorCode code;

    // This can be null, if the error does not come
    // with additional useful information.
    const char* desc;
};

#endif