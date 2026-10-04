#ifndef TB_GEN_HPP
#define TB_GEN_HPP

#include "generator.hpp"
#include <string>
#include <fstream>

void generate_tb(int N, int latency, const std::string& dut);

#endif