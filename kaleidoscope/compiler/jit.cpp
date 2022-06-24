#pragma once

#include "llvm/ADT/StringRef.h"
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
#include "llvm/Support/Error.h"

#include <memory>

#include "gen.cpp"

#ifdef _WIN32
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif

extern "C" DLLEXPORT double printc(double X) {
  fputc((char)X, stdout);
  return 0;
}

namespace jit {

    bool debug = false;

    llvm::Error init();

    // The double ptr will be null if there was no value returned from the evaluated item.
    llvm::Expected<std::unique_ptr<double>> execute(std::unique_ptr<llvm::Module>, std::unique_ptr<llvm::LLVMContext>);

    namespace {
        std::shared_ptr<llvm::DataLayout> layout;
    }

    llvm::Error interactive() {
        printf("IR Generation\n");
        printf("\n");
        printf("Simple Examples:\n");
        printf(" -> 2 + 2\t\t\t(anonymous fn and collasped const)\n");
        printf(" -> extern sin(x); sin(1)\t(externally defined function)\n");
        printf(" -> def f()1; def f()2; f()\t(redefining functions)\n");
        printf(" -> if 1 then 2 else 3\t(control flow)\n");
        printf("\n");
        printf("Extern and Printing:\n");
        printf(" -> extern cos(x); def f(x)sin(x)*sin(x)+cos(x)*cos(x); f(235)\n");
        printf(" -> (for i=0, i<10 in printc(65+i)) + printc(10)\n");
        printf(" -> def f(x) if x then 65 else 90; printc(f(0)) + printc(10)\n");
        printf("\n");
        printf("Operators:\n");
        printf(" -> def binary:1(x y) 0\n");
        printf(" -> def binary~10(a b) a*10 + b\n");
        printf(" -> def unary!(n)if n < 2 then 1 else n*!(n-1)\n");
        printf("\n");

        expr::debug = true;
        gen::debug = true;
        debug = true;

        expr::init();
        if (auto error = gen::init())
            return error;
        if (auto error = init())
            return error;

        while(expr::has_next()) {
            expr::input("jit");
            if (!expr::current)
                continue;
            
            gen::emit(*expr::current, layout);
            if (!gen::has_current())
                continue;

            auto result = execute(std::move(gen::take_module()), std::move(gen::take_context()));
            if (!result)
                return result.takeError();
        }

        return llvm::Error::success();
    }

    namespace {
        std::unique_ptr<llvm::orc::ExecutionSession> session;
        std::unique_ptr<llvm::orc::MangleAndInterner> mangle;
        std::unique_ptr<llvm::orc::RTDyldObjectLinkingLayer> obj_layer;
        std::unique_ptr<llvm::orc::IRCompileLayer> compile_layer;

        std::map<std::string, llvm::orc::ResourceTrackerSP> module_trackers;

        const std::string LIB_NAME = "<main>";
    }

    llvm::Expected<llvm::DataLayout&> get_layout() {
        if (!layout) {
            std::string msg = "No DataLayout found. Has jit::init() been called?\n";
            std::error_code code(1, std::system_category());
            return llvm::make_error<llvm::StringError>(code, msg);
        }
        return *layout;
    }

    llvm::Error init() {
        auto control = llvm::orc::SelfExecutorProcessControl::Create();
        if (!control)
            return control.takeError();

        session = std::make_unique<llvm::orc::ExecutionSession>(std::move(*control));
        llvm::orc::JITTargetMachineBuilder builder(session->getExecutorProcessControl().getTargetTriple());

        auto expected_layout = builder.getDefaultDataLayoutForTarget();
        if (!expected_layout)
            return expected_layout.takeError();
        layout = std::make_unique<llvm::DataLayout>(expected_layout.get());

        mangle = std::make_unique<llvm::orc::MangleAndInterner>(*session, *layout);
        obj_layer = std::make_unique<llvm::orc::RTDyldObjectLinkingLayer>(*session, [](){
            return std::make_unique<llvm::SectionMemoryManager>();
        });
        compile_layer = std::make_unique<llvm::orc::IRCompileLayer>(*session, *obj_layer, 
            std::make_unique<llvm::orc::ConcurrentIRCompiler>(std::move(builder))
        );

        // The lib doesn't need to be stored, it can be fetched by name later.
        // Names are required to be unique, so that isn't a concern.
        session
            ->createBareJITDylib(LIB_NAME)
            .addGenerator(llvm::cantFail(llvm::orc::DynamicLibrarySearchGenerator::GetForCurrentProcess(layout->getGlobalPrefix())));
        
        if (builder.getTargetTriple().isOSBinFormatCOFF()) {
            obj_layer->setOverrideObjectFlagsWithResponsibilityFlags(true);
            obj_layer->setAutoClaimResponsibilityForObjectSymbols(true);
        }

        return llvm::Error::success();
    }

    llvm::Expected<std::unique_ptr<double>> execute(std::unique_ptr<llvm::Module> mod, std::unique_ptr<llvm::LLVMContext> context) {
        std::string name = mod->getName().str();
        
        llvm::orc::ResourceTrackerSP tracker = session->getJITDylibByName(LIB_NAME)->createResourceTracker();
        llvm::orc::ThreadSafeModule thread_safe_mod(std::move(mod), std::move(context));

        auto value_iter = module_trackers.find(name);
        if (value_iter != module_trackers.end())
            value_iter->second->remove();
        module_trackers[name] = tracker;

        if (auto error = compile_layer->add(tracker, std::move(thread_safe_mod))) {
            printf("gen::interactive (add module) -> ");
            return std::move(error);
        }
        
        auto expected_symbol = session->lookup({&*session->getJITDylibByName(LIB_NAME)}, (*mangle)("_main"));
        if (!expected_symbol)
            return nullptr;

        auto symbol = *expected_symbol;
        double (*result_fn)() = (double (*)())(intptr_t)(symbol.getAddress());
        double result = result_fn();

        if (debug) printf("Result: %f\n", result);

        if (auto error = tracker->remove()) {
            printf("gen::interactive: main cleanup -> ");
            return std::move(error);
        }

        return std::make_unique<double>(result);
    }
}
