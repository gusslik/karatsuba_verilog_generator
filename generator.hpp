#ifndef GENERATOR_HPP
#define GENERATOR_HPP

#include <fstream>
#include <string>
#include <set>

const unsigned long BASE = 3;

std::string gen_range(unsigned long width);

std::string gen_range(unsigned long width, unsigned long base);

void gen_child_module_instance(std::ofstream &file, unsigned long width, const char *instance_name, const char *a, const char *b, const char *out);

void gen_mul_module(std::ofstream &file);

std::set<unsigned long> generate_n_set(unsigned long n);

void generate_verilog(std::ofstream& file, unsigned long n);

void generate_base_module(std::ofstream& file, unsigned long n);

void generate_module(std::ofstream& file, unsigned long n);

void generate_karatsuba(std::ofstream& file, std::set<unsigned long>& unique_n, unsigned long n, bool is_top);

#endif