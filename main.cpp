#include <iostream>
#include <fstream>

#include "generator.hpp"

int main(int argc, char *argv[]){
    if(argc == 1){
        std::cerr << "Usage: " << argv[0] << " <N>" << std::endl;
        return EXIT_FAILURE;
    }

    int n = std::stoi(argv[1]);
    if(n < 1){
        std::cerr << "<N> must be more than 0. You entered: " << argv[1] << std::endl;
        return EXIT_FAILURE;
    }

    std::string filename("karatsuba_" + std::string(argv[1]) + ".sv");
    std::ofstream f(filename);

    generate_verilog(f, n);

    return EXIT_SUCCESS;
}