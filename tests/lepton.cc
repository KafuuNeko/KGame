#include "Lepton.h"
#include "lepton/Parser.h"
#include <iostream>
#include <ostream>
#include <string>

int main() {
    std::string expression;

    while (true) {
        std::getline(std::cin, expression);
        if (expression == "exit") {
            break;
        }

        std::cout << Lepton::Parser::parse(expression).evaluate() << std::endl;
    }

    

    return 0;
}
