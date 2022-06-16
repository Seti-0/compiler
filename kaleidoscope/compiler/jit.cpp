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

namespace jit {

    namespace {
        std::unique_ptr<llvm::orc::ExecutionSession> session;
        llvm::DataLayout layout;
        std::unique_ptr<llvm::orc::MangleAndInterner> mangle;
        std::unique_ptr<llvm::orc::RTDyldObjectLinkingLayer> obj_layer;
        std::unique_ptr<llvm::orc::IRCompileLayer> compile_layer;
        std::unique_ptr<llvm::orc::JITDylib&> main;
    }

    llvm::DataLayout& get_layout() {
        return layout;
    }

    llvm::Error addModule(llvm::orc::ThreadSafeModule mod, llvm::orc::ResourceTrackerSP tracker) {
        return compile_layer->add(tracker, std::move(mod));
    }

    llvm::Expected<llvm::JITEvaluatedSymbol> lookup(llvm::StringRef name) {
        return session->lookup({&*main}, (*mangle)(name.str()));
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
        layout = expected_layout.get();

        mangle = std::make_unique<llvm::orc::MangleAndInterner>(*session, layout);
        obj_layer = std::make_unique<llvm::orc::RTDyldObjectLinkingLayer>(*session, [](){
            return std::make_unique<llvm::SectionMemoryManager>();
        });
        compile_layer = std::make_unique<llvm::orc::IRCompileLayer>(session, obj_layer, 
            std::make_unique<llvm::orc::ConcurrentIRCompiler>(std::move(builder))
        );
        main = std::unique_ptr<llvm::orc::JITDylib&>(&session->createBareJITDylib("<main>"));
        main->addGenerator(llvm::cantFail(llvm::orc::DynamicLibrarySearchGenerator::GetForCurrentProcess(layout.getGlobalPrefix())));
        
        if (builder.getTargetTriple().isOSBinFormatCOFF()) {
            obj_layer->setOverrideObjectFlagsWithResponsibilityFlags(true);
            obj_layer->setAutoClaimResponsibilityForObjectSymbols(true);
        }

        return llvm::Error::success();
    }
}