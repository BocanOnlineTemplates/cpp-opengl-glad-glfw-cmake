//
// ProjectTemplate
// Bocan Online C++ Template Project
// main.cpp
//

#include <iostream>

#include "Module.hpp"

int main(int argc, char** argv) {

    std::cout << "Bocan Online C++ Template Project" << std::endl;
    std::cout << "Hello World!" << std::endl;
    std::cout << std::endl;

    {
        Module module;
    }

    std::cin.get();
    std::cout << "Goodbye World!" << std::endl;

    return 0;
}
