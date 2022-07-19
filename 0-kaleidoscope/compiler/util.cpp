#pragma once

#include <stdexcept>
#include <string>

// LLVM generates lots of warnings I can't do anything about.
#pragma warning(push, 0)   
#include "llvm/Support/Error.h"
#pragma warning(pop)

namespace util {
    void rethrow(std::string name, std::string msg)
    {
        std::string text = name;
        if (msg.length() > 0)
            text += ": " + msg;

        try {
            std::rethrow_exception(std::current_exception());
        }
        catch(...) {
            std::throw_with_nested(std::runtime_error(text));
        }
    }

    void rethrow(std::string name) {
        rethrow(name, "");
    }

    void init_throw(std::string name, std::string msg) {
        try {
            throw std::runtime_error(msg);
        } 
        catch(...) {
            rethrow(name);
        }
    }

    void print_exception(const std::exception& e, int level) {
        if (level == 0)
            printf("Error: ");
        else 
            printf(" -> ");
        printf(e.what());

        
        try {
            // For some reason, this line below is a compile-time error for me.
            //std::rethrow_if_nested(e);

            // This works instead, from the documentation of std::rethrow_if_nested.
            if (auto p = dynamic_cast<const std::nested_exception*>(std::addressof(e)))
                p->rethrow_nested();
            
            printf("\n");
        } catch (const std::exception& e) {
            print_exception(e, level + 1);
        }
    }

    void print_exception(const std::exception& e) {
        print_exception(e, 0);
    }
}