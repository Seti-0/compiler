#pragma once

#include <memory>
#include <string>
#include <map>

namespace builtins {
    std::map<std::string, std::shared_ptr<std::string>> map;

    void init() {
        // A prelude with simple utilities.
        map["pre"] = std::make_shared<std::string>(R"(
            extern sin(x)
            extern sin(y)
            extern printc(x)

            def unary!(x) 
                if x then 0 
                else 1
            
            def unary-(x) 0-x

            def binary|5(a b) 
                if a then 1 
                else if b then 1 
                else 0
            
            def binary&6(a b) 
            if a then if b then 1;

            def binary>8(a b) b < a
            def binary~7(a b) !(a < b | b > a)
            def binary:1(a b) 0
        )");

        map["mandle"] = std::make_shared<std::string>(R"(

        )");
    }
}