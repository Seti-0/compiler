#pragma once

#include "llvm/Support/Error.h"
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

#include <memory>

#include "gen.cpp"
#include "imports.cpp"

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
    llvm::Expected<std::unique_ptr<double>> execute(std::string promt);

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

        if (auto error = init())
            return error;

        while(tokens::has_next()) {
            auto result = execute("jit");
            if (!result)
                return std::move(result.takeError());
        }

        return llvm::Error::success();
    }

    namespace {
        std::unique_ptr<llvm::orc::ExecutionSession> session;
        std::unique_ptr<llvm::orc::MangleAndInterner> mangle;
        std::unique_ptr<llvm::orc::RTDyldObjectLinkingLayer> obj_layer;
        std::unique_ptr<llvm::orc::IRCompileLayer> compile_layer;

        std::map<std::string, std::unique_ptr<ast::Block>> blocks;
        std::map<std::string, llvm::orc::ResourceTrackerSP> module_trackers;

        const std::string LIB_NAME = "<main>";
    }

    void cleanup() {
        session->getJITDylibByName(LIB_NAME)->clear();
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
        builtins::init();
        if (auto error = gen::init())
            return std::move(error);

        auto control = llvm::orc::SelfExecutorProcessControl::Create();
        if (!control)
            return std::move(control.takeError());

        session = std::make_unique<llvm::orc::ExecutionSession>(std::move(*control));
        llvm::orc::JITTargetMachineBuilder builder(session->getExecutorProcessControl().getTargetTriple());

        auto expected_layout = builder.getDefaultDataLayoutForTarget();
        if (!expected_layout)
            return std::move(expected_layout.takeError());
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

    llvm::Expected<std::unique_ptr<double>> execute(std::unique_ptr<ast::Block>);
    llvm::Expected<std::unique_ptr<double>> execute(std::string promt) {
        expr::input(promt);
        if (!expr::current)
            return nullptr;
        
        return std::move(execute(std::move(expr::current)));
    }

    void execute_builtin(std::string key) {
        if (builtins::map.count(key) > 0) {
            tokens::chars::set_source_text(builtins::map[key]);
        }
        else {
            printf("Import not found: '%s'\n", key.c_str());
            return;
        }

        while(tokens::has_next())
            execute("");
        
        tokens::chars::reset_source();
    }

    llvm::Error compile(ast::Item& item, llvm::orc::ResourceTrackerSP tracker);
    llvm::Expected<std::unique_ptr<double>> execute(std::unique_ptr<ast::Block> block) {
        if (ast::Import* import = block->as_import()) {
            execute_builtin(import->file);
            return nullptr;
        }

        // Before executing, remove existing symbols that conflict. (They are being redefined)
        // Unfortunately, I think this means removing the entire module with the conflict, and 
        // recompiling the parts that don't conflict.

        // Deal with top level expressions (anonymous functions) separately.
        std::vector<std::unique_ptr<ast::Fn>> anonymous_functions;
        llvm::orc::ResourceTrackerSP new_tracker = session->getJITDylibByName(LIB_NAME)->createResourceTracker();
        for (auto it = block->statements.begin(); it != block->statements.end();) {
            ast::Fn* new_fn = (*it)->as_fn();
            if (!new_fn) {
                it++;
                continue;
            }

            // Anonymous functions (representing top-level expressions) never clash since
            // they are dealth with separately and never stored.
            if (new_fn->proto->name == "_main") {
                std::unique_ptr<ast::Fn> taken_fn = std::unique_ptr<ast::Fn>((ast::Fn*)(*it).release());
                it = block->statements.erase(it);
                anonymous_functions.push_back(std::move(taken_fn));
                continue;
            }

            // For non-anonymous functions, remove any previously compiled code.
            // Unfortunately this also removes any code originally associated with that function.
            auto tracker_iter = module_trackers.find(new_fn->proto->name);
            bool tracker_found = false;
            if (tracker_iter != module_trackers.end()) {
                tracker_iter->second->remove();
                tracker_found = true;
            }

            // Now, to recompile the bits (if any) that were removed by association but
            // aren't up for replacement. (We still might need them!)
            auto block_iter = blocks.find(new_fn->proto->name);
            if (block_iter != blocks.end()) {
                std::unique_ptr<ast::Block> to_recompile = std::move(block_iter->second);
                llvm::orc::ResourceTrackerSP replacement_tracker = session->getJITDylibByName(LIB_NAME)->createResourceTracker();
                
                // Before recompiling, remove the function to be replaced from the block.
                // (Otherwise this will have been for nothing.)
                for (auto it = block->statements.begin(); it != block->statements.end();) {
                    if (ast::Fn* old_fn = (*it)->as_fn()) {
                        if (old_fn->proto->name == new_fn->proto->name)
                            it = block->statements.erase(it);
                        else {
                            // Also update the tracker ref. while we're at it.
                            module_trackers[old_fn->proto->name] = replacement_tracker;
                            it++;
                        }
                    }
                }

                if (auto error = compile(*to_recompile, replacement_tracker))
                    return std::move(error);
            }

            // Finally, register a new tracker for the function that will be
            // redefined in a moment.
            module_trackers[new_fn->proto->name] = new_tracker;  

            it++;
        }

        // Now that there are no conflicts, and the anonymous parts have been removed,
        // and the tracker and block maps have been updated, execute the original block.
        if (block->statements.size() > 0)
            compile(*block, new_tracker);

        // Next, run through the anonymous functions one by one. They need to be compiled 
        // and executed and released.
        std::unique_ptr<double> result = nullptr;
        for (std::unique_ptr<ast::Fn>& fn: anonymous_functions) {
            llvm::orc::ResourceTrackerSP temp_tracker = session->getJITDylibByName(LIB_NAME)->createResourceTracker();
            if (auto error = compile(*fn, temp_tracker))
                return std::move(error);

            auto expected_symbol = session->lookup({&*session->getJITDylibByName(LIB_NAME)}, (*mangle)("_main"));
            if (!expected_symbol) {
                printf("WARNING: Unable to find symbol '_main' after compiling anonymous function.\n");
                continue;
            }

            auto symbol = *expected_symbol;
            double (*result_fn)() = (double (*)())(intptr_t)(symbol.getAddress());
            result = std::make_unique<double>(result_fn());

            if (debug) printf("Result: %f\n", *result);

            if (auto error = temp_tracker->remove()) {
                printf("gen::interactive: main cleanup -> ");
                return std::move(error);
            }
        }

        return std::move(result);
    }

    llvm::Error compile(ast::Item& item, llvm::orc::ResourceTrackerSP tracker) {
        gen::emit(item, layout);
        if (!gen::has_current()) {
            // TODO: Maybe thing about adding a useful name here.
            printf("WARNING: failed to regen IR for module.\n");
            return llvm::Error::success();
        }

        llvm::orc::ThreadSafeModule thread_safe_mod(std::move(gen::take_module()), std::move(gen::take_context()));

        if (auto error = compile_layer->add(tracker, std::move(thread_safe_mod))) {
            printf("gen::interactive (replace existing module) -> ");
            return std::move(error);
        }

        return llvm::Error::success();
    }
}
