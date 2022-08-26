// This contains a minimal example of using the LLVM JIT to
// call an external function and print a message to the screen.
// It was used to check the CMakeLists.txt from time to time.

// LLVM generates lots of warnings I can't do anything about.
#pragma warning(push, 0)   
#include "llvm/Support/Error.h"
#include "llvm/ExecutionEngine/JITSymbol.h"
#include "llvm/ExecutionEngine/SectionMemoryManager.h"
#include "llvm/ExecutionEngine/Orc/CompileUtils.h"
#include "llvm/ExecutionEngine/Orc/Core.h"
#include "llvm/ExecutionEngine/Orc/ExecutionUtils.h"
#include "llvm/ExecutionEngine/Orc/ExecutorProcessControl.h"
#include "llvm/ExecutionEngine/Orc/IRCompileLayer.h"
#include "llvm/ExecutionEngine/Orc/JITTargetMachineBuilder.h"
#include "llvm/ExecutionEngine/Orc/Mangling.h"
#include "llvm/ExecutionEngine/Orc/RTDyldObjectLinkingLayer.h"
#include "llvm/ExecutionEngine/Orc/ThreadSafeModule.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/Support/Host.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/ADT/Triple.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Verifier.h"
#pragma warning(pop)

#include <iostream>

#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

extern "C" EXPORT double test() {
    printf("Hello world from an external function!\n");
    return 42;
}

int main() {
    auto exit_on_error = llvm::ExitOnError();

    // ############
    // # Init JIT #
    // ############

    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();

    auto control = llvm::orc::SelfExecutorProcessControl::Create();
    if (!control) exit_on_error(control.takeError());

    llvm::orc::ExecutionSession session{std::move(*control)};
    llvm::Triple triple = session.getExecutorProcessControl().getTargetTriple();
    llvm::orc::JITTargetMachineBuilder builder(triple);

    auto expected_layout = builder.getDefaultDataLayoutForTarget();
    if (!expected_layout) exit_on_error(expected_layout.takeError());
    auto layout = expected_layout.get();

    llvm::orc::MangleAndInterner mangle{session, layout};
    llvm::orc::RTDyldObjectLinkingLayer obj_layer {session, [](){
        return std::make_unique<llvm::SectionMemoryManager>();
    }};
    llvm::orc::IRCompileLayer compile_layer {session, obj_layer,
        std::make_unique<llvm::orc::ConcurrentIRCompiler>(std::move(builder))
    };

    std::string LIB_NAME = "main";
    session.createBareJITDylib(LIB_NAME)
        .addGenerator(llvm::cantFail(llvm::orc::DynamicLibrarySearchGenerator::GetForCurrentProcess(layout.getGlobalPrefix())));
    
    if (builder.getTargetTriple().isOSBinFormatCOFF()) {
        obj_layer.setOverrideObjectFlagsWithResponsibilityFlags(true);
        obj_layer.setAutoClaimResponsibilityForObjectSymbols(true);
    }

    // ###############
    // # Generate IR #
    // ###############

    std::unique_ptr<llvm::LLVMContext> context = std::make_unique<llvm::LLVMContext>();
    std::unique_ptr<llvm::Module> mod = std::make_unique<llvm::Module>("main", *context);
    mod->setDataLayout(layout);
    mod->setTargetTriple(triple.getTriple());

    // Create the main function.
    std::vector<llvm::Type*> args;
    llvm::Type* ret_type = llvm::Type::getDoubleTy(*context);
    llvm::FunctionType* fn_type = llvm::FunctionType::get(ret_type, args, false);
    llvm::Function* fn = llvm::Function::Create(fn_type, llvm::Function::ExternalLinkage, "main", *mod);
    llvm::Function* extern_fn = llvm::Function::Create(fn_type, llvm::Function::ExternalLinkage, "test", *mod);

    llvm::BasicBlock* block = llvm::BasicBlock::Create(*context, "enter", fn);
    llvm::IRBuilder<> ir_builder{*context};
    ir_builder.SetInsertPoint(block);

    std::vector<llvm::Value*> call_args;
    llvm::Value* val = ir_builder.CreateCall(extern_fn, call_args, "val");

    ir_builder.CreateRet(val);
    llvm::verifyFunction(*fn);

    printf("IR: \n");
    mod->print(llvm::outs(), nullptr);

    // ########################
    // # Compile & Execute IR #
    // ########################

    llvm::orc::ResourceTrackerSP tracker = session.getJITDylibByName(LIB_NAME)->createResourceTracker();
    llvm::orc::ThreadSafeModule thread_safe_mod(std::move(mod), std::move(context));
    if (auto error = compile_layer.add(tracker, std::move(thread_safe_mod))) {
        exit_on_error(std::move(error));
    }
    auto expected_symbol = session.lookup({&*session.getJITDylibByName(LIB_NAME)}, (mangle)("main"));
    if (!expected_symbol) exit_on_error(expected_symbol.takeError());
    double (*main_fn)() = (double (*)())(intptr_t)((*expected_symbol).getAddress());
    double result = main_fn();

    printf("RESULT: '%f'\n", result);
    
    // ###########
    // # Cleanup #
    // ###########

    session.getJITDylibByName(LIB_NAME)->clear();
}