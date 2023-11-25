#include <string>
#include <iostream>

namespace test {

// The diff algorithm is based on a well-known paper called 
// called "An O(ND) Difference Algorithm and Its Variations" by Eugene Myers.

// I made a separate Python notebook exploring it and explaining what this
// actually does.

void diff(std::string a, std::string b) {
    int N = a.length();
    int M = b.length();

    int D = 0;
    std::vector<int> steps;
    std::vector<int> positions;

    while (D < N + M) {

    }
}    

}