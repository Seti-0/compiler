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
    class JIT {
    private:
        std::unique_ptr<llvm::orc::ExecutionSession> session;
        llvm::DataLayout layout;
        llvm::orc::MangleAndInterner mangle;
        llvm::orc::RTDyldObjectLinkingLayer obj_layer;
        llvm::orc::IRCompileLayer compile_layer;
        llvm::orc::JITDylib& main;

    public:
        JIT(std::unique_ptr<llvm::orc::ExecutionSession> session,
            llvm::orc::JITTargetMachineBuilder builder,
            llvm::DataLayout layout
        ): session(std::move(session)),
        layout(std::move(layout)),
        mangle(*this->session, this->layout),
        obj_layer(*this->session, [](){
            return std::make_unique<llvm::SectionMemoryManager>();
        }),
        compile_layer(*this->session, obj_layer, 
            std::make_unique<llvm::orc::ConcurrentIRCompiler>(std::move(builder))
        ),
        main(this->session->createBareJITDylib("<main>")) {
            main.addGenerator(llvm::cantFail(llvm::orc::DynamicLibrarySearchGenerator::GetForCurrentProcess(layout.getGlobalPrefix())));
            if (builder.getTargetTriple().isOSBinFormatCOFF()) {
                obj_layer.setOverrideObjectFlagsWithResponsibilityFlags(true);
                obj_layer.setAutoClaimResponsibilityForObjectSymbols(true);
            }
        }

        ~JIT() {
            if(llvm::Error error = session->endSession())
                session->reportError(std::move(error));
        }

        static llvm::Expected<std::unique_ptr<JIT>> create() {
            auto control = llvm::orc::SelfExecutorProcessControl::Create();
            if (!control)
                return control.takeError();
            
            auto session = std::make_unique<llvm::orc::ExecutionSession>(std::move(*control));
            llvm::orc::JITTargetMachineBuilder builder(session->getExecutorProcessControl().getTargetTriple());

            auto layout = builder.getDefaultDataLayoutForTarget();
            if (!layout)
                return layout.takeError();
            
            return std::make_unique<JIT>(std::move(session), builder, layout.get());
        }

        const llvm::DataLayout& getDataLayout() const {
            return layout;
        }

        llvm::orc::JITDylib& getMainJITDyLib() {
            return main;
        }

        llvm::Error addModule(
            llvm::orc::ThreadSafeModule mod, 
            llvm::orc::ResourceTrackerSP tracker=nullptr
        ) {
            if (!tracker)
                tracker = main.getDefaultResourceTracker();
            
            return compile_layer.add(tracker, std::move(mod));
        }

        llvm::Expected<llvm::JITEvaluatedSymbol> lookup(llvm::StringRef name) {
            return session->lookup({&main}, mangle(name.str()));
        }
    };
}