
#include "scanner.h"
#include <cstdio>
#include <fstream>
#include <iostream>
#include <stdexcept>

using Parser = yy::parser;

int main(int argc, char ** argv)
{
    Scanner scanner;
    Parser parser(scanner);

    std::ifstream file (argv[1]);
    try {
        if (file.is_open())
            scanner.yyrestart(&file);
        else 
            throw std::runtime_error("No such file!");
        int res = parser();
        return res;
    }
    catch(std::runtime_error &e) {
        std::cout << e.what() << std::endl;
    }
}