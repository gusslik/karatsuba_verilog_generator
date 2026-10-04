#include "generator.hpp"
#include <iostream>

struct Split
{
    unsigned long h, l, s;
};

Split new_split(unsigned long n)
{
    Split split;
    unsigned long h, l, s;
    h = n / 2;
    l = n - h;
    s = l + 1;

    split.h = h;
    split.l = l;
    split.s = s;

    return split;
}

std::string gen_range(unsigned long width)
{
    return "[" + std::to_string(width - 1) + ":0]";
}

std::string gen_range(unsigned long width, unsigned long base)
{
    return "[" + std::to_string(width - 1) + ":" + std::to_string(base) + "]";
}

void gen_child_module_instance(std::ofstream &file, unsigned long width, const char *instance_name, const char *a, const char *b, const char *out)
{
    if (width <= BASE)
    {
        file << "\tmul #(.W(" << width << ")) ";
    }
    else
    {
        file << "\tkaratsuba_" << width << " ";
    }

    file << instance_name << "(.a(" << a << "), .b(" << b << "), .out(" << out << "));\n";
}

void gen_mul_module(std::ofstream &file)
{
    file << "module mul #(parameter W = 2)(\n"
         << "\tinput logic [W-1:0] a,\n"
         << "\tinput logic [W-1:0] b,\n"
         << "\toutput logic [2*W-1:0] out\n"
         << ");\n"
         << "\tlogic [W-1:0] partial [W-1:0];\n"
         << "\tgenvar i;\n"
         << "\tgenerate\n"
         << "\t\tfor(i = 0; i < W; i++) begin\n"
         << "\t\t\tassign partial[i] = a & {W{b[i]}};\n"
         << "\t\tend\n"
         << "\tendgenerate\n"
         << "\talways_comb begin\n"
         << "\t\tout = '0;\n"
         << "\t\tfor(int i = 0; i < W; i++) begin\n"
         << "\t\t\tout = out + ({{W{1'b0}}, partial[i]} << i);\n"
         << "\t\tend\n"
         << "\tend\n"
         << "endmodule\n\n";
}

void generate_verilog(std::ofstream &file, unsigned long n)
{
    std::set<unsigned long> unique_n;
    gen_mul_module(file);

    generate_karatsuba(file, unique_n, n, true);
}

void generate_module(std::ofstream &file, unsigned long n)
{

    const Split split = new_split(n);

    file << "module karatsuba_" << n << "(\n"
         << "\tinput logic " << gen_range(n) << " a,\n"
         << "\tinput logic " << gen_range(n) << " b,\n"
         << "\toutput logic " << gen_range(2 * n) << " out\n"
         << ");\n"
         << "\tlogic " << gen_range(split.h) << " a_lo, b_lo;\n"
         << "\tlogic " << gen_range(split.l) << " a_hi, b_hi;\n"
         << "\tlogic " << gen_range(split.s) << " a_sum, b_sum;\n";

    file << "\tassign a_hi = a" << gen_range(n, split.h) << ";\n"
         << "\tassign a_lo = a" << gen_range(split.h, 0) << ";\n"
         << "\tassign b_hi = b" << gen_range(n, split.h) << ";\n"
         << "\tassign b_lo = b" << gen_range(split.h, 0) << ";\n"
         << "\tassign a_sum = a_hi + a_lo;\n"
         << "\tassign b_sum = b_hi + b_lo;\n";

    file << "\tlogic " << gen_range(2 * split.h) << " z0;\n"
         << "\tlogic " << gen_range(2 * split.l) << " z2;\n"
         << "\tlogic " << gen_range(2 * split.s) << " zm;\n";

    gen_child_module_instance(file, split.h, "u0", "a_lo", "b_lo", "z0");
    gen_child_module_instance(file, split.l, "u2", "a_hi", "b_hi", "z2");
    gen_child_module_instance(file, split.s, "um", "a_sum", "b_sum", "zm");

    file << "\tlogic" << gen_range(2 * split.s) << " z1;\n"
         << "\tassign z1 = zm - z0 - z2;\n"
         << "\tassign out = (z2 << " << 2 * split.h << ") + (z1 << " << split.h << ") + z0;\n";

    file << "endmodule" << std::endl;
}

void generate_ff_module(std::ofstream &file, unsigned long n)
{
    const Split split = new_split(n);

    file << "module karatsuba_" << n << "(\n"
         << "\tinput logic clk,\n"
         << "\tinput logic " << gen_range(n) << " a,\n"
         << "\tinput logic " << gen_range(n) << " b,\n"
         << "\toutput logic " << gen_range(2 * n) << " out\n"
         << ");\n"
         << "\tlogic " << gen_range(split.h) << " a_lo_r, b_lo_r;\n"
         << "\tlogic " << gen_range(split.l) << " a_hi_r, b_hi_r;\n"
         << "\tlogic " << gen_range(split.s) << " a_sum_r, b_sum_r;\n";

    file << "\talways_ff @(posedge clk) begin\n"
         << "\t\ta_hi_r <= a" << gen_range(n, split.h) << ";\n"
         << "\t\ta_lo_r <= a" << gen_range(split.h, 0) << ";\n"
         << "\t\tb_hi_r <= b" << gen_range(n, split.h) << ";\n"
         << "\t\tb_lo_r <= b" << gen_range(split.h, 0) << ";\n"
         << "\t\ta_sum_r <= a" << gen_range(n, split.h) << " + a" << gen_range(split.h, 0) << ";\n"
         << "\t\tb_sum_r <= b" << gen_range(n, split.h) << " + b" << gen_range(split.h, 0) << ";\n"
         << "\tend\n";

    file << "\tlogic " << gen_range(2 * split.h) << " z0_m;\n"
         << "\tlogic " << gen_range(2 * split.l) << " z2_m;\n"
         << "\tlogic " << gen_range(2 * split.s) << " zm_m;\n";

    gen_child_module_instance(file, split.h, "u0", "a_lo_r", "b_lo_r", "z0_m");
    gen_child_module_instance(file, split.l, "u2", "a_hi_r", "b_hi_r", "z2_m");
    gen_child_module_instance(file, split.s, "um", "a_sum_r", "b_sum_r", "zm_m");

    file << "\tlogic " << gen_range(2 * split.h) << " z0_r;\n"
         << "\tlogic " << gen_range(2 * split.l) << " z2_r;\n"
         << "\tlogic " << gen_range(2 * split.s) << " zm_r;\n";

    file << "\talways_ff @(posedge clk) begin\n"
         << "\t\tz0_r <= z0_m;\n"
         << "\t\tz2_r <= z2_m;\n"
         << "\t\tzm_r <= zm_m;\n"
         << "\tend\n";

    file << "\tlogic " << gen_range(2 * split.s) << " z1;\n"
         << "\tassign z1 = zm_r - z0_r - z2_r;\n";

    file << "\talways_ff @(posedge clk)\n"
         << "\t\tout <= (z2_r << " << 2 * split.h << ") + (z1 << " << split.h << ") + z0_r;\n";

    file << "endmodule" << std::endl;
}

void generate_small_ff_module(std::ofstream &file, unsigned long n)
{
    file << "module karatsuba_" << n << "(\n"
         << "\tinput logic clk,\n"
         << "\tinput logic " << gen_range(n) << " a,\n"
         << "\tinput logic " << gen_range(n) << " b,\n"
         << "\toutput logic " << gen_range(2 * n) << " out\n"
         << ");\n";

    file << "\tlogic " << gen_range(n) << " a_r, b_r;\n"
         << "\tlogic " << gen_range(2 * n) << " p, p_r;\n";

    gen_child_module_instance(file, n, "up", "a_r", "b_r", "p");

    file << "\talways_ff @(posedge clk) begin\n"
         << "\t\ta_r <= a;\n"
         << "\t\tb_r <= b;\n"
         << "\t\tp_r <= p;\n"
         << "\t\tout <= p_r;\n"
         << "\tend\n";

    file << "endmodule" << std::endl;
}

void generate_karatsuba(std::ofstream &file, std::set<unsigned long> &unique_n, unsigned long n, bool is_top = false)
{

    if (!unique_n.insert(n).second)
    {
        return;
    }

    if (is_top)
    {
        if (n <= BASE)
        {
            generate_small_ff_module(file, n);
        }
        else
        {
            generate_ff_module(file, n);
        }
    }
    else
    {
        if (n <= BASE)
        {

            return;
        }

        generate_module(file, n);
    }

    unsigned long low, high, next_n;
    if (n % 2 == 0)
    {
        low = n / 2;
        high = n / 2;
        next_n = high + 1;
    }
    else
    {
        low = n / 2;
        high = (n / 2) + 1;
        next_n = high + 1;
    }

    generate_karatsuba(file, unique_n, low);
    generate_karatsuba(file, unique_n, high);
    generate_karatsuba(file, unique_n, next_n);
}