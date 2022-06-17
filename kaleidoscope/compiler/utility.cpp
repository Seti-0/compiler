#pragma once

#include <stdexcept>
#include <string>

#include "llvm/Support/Error.h"

namespace util {
    void initial_throw(std::string name, std::string msg) {
        throw std::runtime_error(name + ": " + msg);
    }

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

    void print_exception(const std::exception& e, int level) {
        if (level == 0)
            printf("Error: ");
        else 
            printf(" -> ");
        printf(e.what());
        
        try {
            std::rethrow_if_nested(e);
            printf("\n");
        } catch (const std::exception& e) {
            print_exception(e, level + 1);
        }
    }

    void print_exception(const std::exception& e) {
        print_exception(e, 0);
    }
}