#include <iostream>
#include <fstream>

#include "generator.hpp"

int main(int argc, char *argv[]){
    if(argc == 1){
        std::cerr << "Usage: " << argv[0] << " <N>" << std::endl;
        return EXIT_FAILURE;
    }

    create_file("file.txt");

    return EXIT_SUCCESS;
}