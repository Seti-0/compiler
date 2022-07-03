#pragma once

// LLVM generates lots of warnings I can't do anything about.
#pragma warning(push, 0)   
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
#pragma warning(pop)

#include <memory>

#include "gen.cpp"
#include "imports.cpp"

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

        std::map<std::string, std::shared_ptr<ast::Block>> associations;
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
        if (builtins::map.count(key) == 0) {
            printf("Import not found: '%s'\n", key.c_str());
            return;
        }

        std::istringstream stream(*builtins::map[key]);
        tokens::set_input(stream);
        expr::interactive_mode = false;

        while(tokens::has_next())
            execute("");
        
        tokens::set_input(std::cin);
        expr::interactive_mode = true;
    }

    llvm::Error compile(ast::Item& item, llvm::orc::ResourceTrackerSP tracker);
    void execute_externs(std::vector<std::unique_ptr<ast::Statement>> externs);
    llvm::Error compile_functions(std::vector<std::unique_ptr<ast::Fn>> functions);
    llvm::Expected<std::unique_ptr<double>> execute_anonymous_fn(ast::Fn& fn);

    llvm::Expected<std::unique_ptr<double>> execute(std::unique_ptr<ast::Block> unique_block) {
        std::shared_ptr<ast::Block> block = std::shared_ptr<ast::Block>(unique_block.release());

        std::unique_ptr<double> result = nullptr;

        // Multiple (non-main) functions in a row are grouped into a single module for neatness.
        // This does make things complicated when a function within a group is redefined, since
        // I think only modules as a whole can be compiled/recompiled.
        std::vector<std::unique_ptr<ast::Fn>> functions;

        // Extern defs are grouped too, but this is just for the sake of neat output.
        // No actual IR is generated for extern statements, they just register names.
        std::vector<std::unique_ptr<ast::Statement>> externs;

        for (std::unique_ptr<ast::Statement>& statement: block->statements) {
            ast::Fn* fn = statement->as_fn();
            if (fn && fn->proto->name != "_main") {
                std::unique_ptr<ast::Fn> taken_fn = std::unique_ptr<ast::Fn>((ast::Fn*)statement.release());
                functions.push_back(std::move(taken_fn));
            }
            else if (ast::Pro* pro = statement->as_pro()) {
                std::unique_ptr<ast::Pro> taken_pro = std::unique_ptr<ast::Pro>((ast::Pro*)statement.release());
                externs.push_back(std::move(taken_pro));
            }
            else {
                // These other actions do require that any pending functions have been
                // compiled first.
                if (externs.size() > 0) {
                    execute_externs(std::move(externs));
                    externs = std::vector<std::unique_ptr<ast::Statement>>();
                }

                if (functions.size() > 0) {
                    compile_functions(std::move(functions));
                    // Is std::move guaranteed to leave a valid (but cleared) vector,
                    // or must a new one be initialized?
                    functions = std::vector<std::unique_ptr<ast::Fn>>();
                }

                if (fn && fn->proto->name == "_main") {
                    llvm::Expected<std::unique_ptr<double>> expected = execute_anonymous_fn(*fn);
                    if (!expected) return expected.takeError();
                    result = std::move(*expected);
                }
                else if (ast::Import* import = statement->as_import()) {
                    execute_builtin(import->file);
                }
            }
        }
        // Compile any pending functions.
        execute_externs(std::move(externs));
        compile_functions(std::move(functions));

        return std::move(result);
    }

    void execute_externs(std::vector<std::unique_ptr<ast::Statement>> externs) {
        if (externs.size() == 0)
            return;

        if (debug) printf("Declaring %zd external symbol(s).\n", externs.size());
        gen::emit(ast::Block(std::move(externs)), layout);
    }

    llvm::Error compile_functions(std::vector<std::unique_ptr<ast::Fn>> functions) {
        if (functions.size() == 0)
            return llvm::Error::success();

        if (debug) printf("Compiling %zd function(s).\n", functions.size());

        // Remove old compiled code, find all functions that were removed by association.

        std::vector<std::shared_ptr<ast::Block>> to_compile;
        for (std::unique_ptr<ast::Fn>& new_fn: functions) {
            auto tracker_iter = module_trackers.find(new_fn->proto->name);
            if (tracker_iter != module_trackers.end()) {
                tracker_iter->second->remove();
            }

            auto assoc_iter = associations.find(new_fn->proto->name);
            if (assoc_iter != associations.end()) {
                to_compile.push_back(assoc_iter->second);
            }
        }

        // Remove association between any new function and previous functions, the new
        // functions will form a new associated group.

        for (std::unique_ptr<ast::Fn>& new_fn: functions) {
            for (std::shared_ptr<ast::Block>& block: to_compile) {
                for (auto it = block->statements.begin(); it != block->statements.end();) {
                    ast::Fn* fn = (*it)->as_fn();
                    if (!fn) {
                        std::error_code code(1, std::system_category());
                        return llvm::make_error<llvm::StringError>(code, "Internal error: non-function statement stored in an associated function block.");
                    }

                    if (fn->proto->name == new_fn->proto->name) {
                        it = block->statements.erase(it);
                    }
                    else {
                        it++;
                    }
                }
            }
        }

        // Create a block to compile for the new functions, and a tracker.
        // (Is it possible to just cast A<B*> to A<C*> where B:C?)

        std::vector<std::unique_ptr<ast::Statement>> statements;
        std::shared_ptr<ast::Block> new_block = std::make_shared<ast::Block>(std::move(statements));
        
        for (std::unique_ptr<ast::Fn>& new_fn: functions) {
            associations[new_fn->proto->name] = new_block;
            new_block->statements.push_back(std::move(new_fn));
        }
        to_compile.push_back(new_block);

        // Compile all blocks, update all trackers.

        for (std::shared_ptr<ast::Block>& block: to_compile) {
            if (block->statements.size() == 0)
                continue;
            
            llvm::orc::ResourceTrackerSP tracker = session->getJITDylibByName(LIB_NAME)->createResourceTracker();
            for (std::unique_ptr<ast::Statement>& statement: block->statements) {
                if (ast::Fn* fn = statement->as_fn()) {
                    module_trackers[fn->proto->name] = tracker;
                }
            }

            compile(*block, tracker);
        }

        return llvm::Error::success();
    }

    llvm::Expected<std::unique_ptr<double>> execute_anonymous_fn(ast::Fn& fn) {
        llvm::orc::ResourceTrackerSP temp_tracker = session->getJITDylibByName(LIB_NAME)->createResourceTracker();
        if (auto error = compile(fn, temp_tracker))
            return std::move(error);

        auto expected_symbol = session->lookup({&*session->getJITDylibByName(LIB_NAME)}, (*mangle)("_main"));
        if (!expected_symbol) {
            printf("WARNING: Unable to find symbol '_main' after compiling anonymous function.\n");
            return nullptr;
        }

        auto symbol = *expected_symbol;
        double (*result_fn)() = (double (*)())(intptr_t)(symbol.getAddress());
        std::unique_ptr<double> result = std::make_unique<double>(result_fn());

        if (debug) printf("Result: %f\n", *result);

        if (auto error = temp_tracker->remove()) {
            printf("gen::interactive: main cleanup -> ");
            return std::move(error);
        }

        return result;
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
