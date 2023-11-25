#include "../compiler/tokens.cpp"

#include <iostream>
#include <filesystem>
#include "diff.cpp"

namespace fs = std::filesystem;

int main() {

    test::diff("one", "two");
    return 0;

    printf("test-tokens v1\n");

    fs::path target = fs::path("./tests/samples/tokens.river");
    
    if (!fs::exists(target)) {
        std::cout << "Target file " << target << " does not seem to exist." << std::endl;
        return 1;
    }

    if (!fs::is_regular_file(target)) {
        std::cout << "Target file " << target << " does not seem to be a regular file." << std::endl;
        return 1;
    }

    std::ifstream stream(target);
    tokens::TokenIterator iter(stream);
    while (iter.has_next()) {
        iter.next();
        std::cout << "Token: " << iter.peek().describe() << std::endl;
    }
}
