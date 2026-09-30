#include "generator.hpp"

void create_file(const std::string &filename){
    std::ofstream f;
    f.open(filename);
    if(f.is_open()){
        f << "Hello World!" << std::endl;
    }

    f.close();
}