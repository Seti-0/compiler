#pragma once

#include <memory>
#include <string>
#include <map>

namespace builtins {
    std::map<std::string, std::shared_ptr<std::string>> map;

    void init() {
        map["pre"] = std::make_shared<std::string>(R"(
            def a() 1
            def b() 2
            a() + b()
        )");
    }
}