#include <string>

// LLVM generates lots of warnings I can't do anything about.
#pragma warning(push, 0)    
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/Host.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/Support/TargetSelect.h"
#pragma warning(pop)

#include "ir-gen.h"

void LW_Init() {
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
}

LW_TargetMachine* LW_GetDefaultMachine() {
    std::error_code ERROR_CODE(1, std::system_category());

    std::string triple_text = llvm::sys::getDefaultTargetTriple();
    std::string error;
    const llvm::Target* target = llvm::TargetRegistry::lookupTarget(triple_text, error);
    if (!target)
        llvm::make_error<llvm::StringError>(ERROR_CODE, error);

    std::string cpu = "";
    std::string features = "";
    llvm::TargetOptions options;
    auto model = llvm::Optional<llvm::Reloc::Model>();
    llvm::TargetMachine* machine = target->createTargetMachine(triple_text, cpu, features, options, model);

    return reinterpret_cast<LW_TargetMachine*>(machine);
}

const char* LW_TargetMachine_GetDescription(LW_TargetMachine* lw_target) {
    llvm::TargetMachine* target = reinterpret_cast<llvm::TargetMachine*>(lw_target);
    
    const llvm::Triple& machine_triple = target->getTargetTriple();
    return machine_triple.getTriple().c_str();
}

void test() {
    

    std::error_code ERROR_CODE(1, std::system_category());

    std::string triple_text = llvm::sys::getDefaultTargetTriple();
    std::string error;
    const llvm::Target* target = llvm::TargetRegistry::lookupTarget(triple_text, error);
    if (!target)
        llvm::make_error<llvm::StringError>(ERROR_CODE, error);

    std::string cpu = "";
    std::string features = "";
    llvm::TargetOptions options;
    auto model = llvm::Optional<llvm::Reloc::Model>();
    llvm::TargetMachine* machine = target->createTargetMachine(triple_text, cpu, features, options, model);

    const llvm::Triple& machine_triple = machine->getTargetTriple();
    const llvm::DataLayout machine_layout = machine->createDataLayout();
}
