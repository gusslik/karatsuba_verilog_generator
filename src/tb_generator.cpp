#include <algorithm>
#include <fstream>
#include <string>

void generate_tb(int N, int latency, const std::string& dut) {
    int tests = 100000;

    if (N > 8)
        tests = std::max(1000, std::min(100000, 100000 / N));
    std::ofstream f("tb_" + dut + ".sv");

    std::string rnd = "{";
    for (int i = 0; i < (N + 31) / 32; i++) rnd += (i ? ", $urandom" : "$urandom");
    rnd += "}";

    const std::string v = (N <= 32) ? "%0d" : "0x%0h";
    const std::string fmt = "a=" + v + " b=" + v + " out=" + v + " expected=" + v;

    f << "`timescale 1ns/1ps\n"
      << "module tb_" << dut << ";\n"
      << "\tlocalparam N   = " << N << ";\n"
      << "\tlocalparam LAT = " << latency << ";\n\n"
      << "\tlogic clk = 0;\n"
      << "\tlogic [N-1:0]   a, b;\n"
      << "\tlogic [2*N-1:0] out;\n\n"
      << "\t" << dut << " dut (.clk(clk), .a(a), .b(b), .out(out));\n"
      << "\talways #5 clk = ~clk;\n\n"
      << "\tlogic [2*N-1:0] expected [LAT];\n"
      << "\tlogic [N-1:0]   hist_a   [LAT];\n"
      << "\tlogic [N-1:0]   hist_b   [LAT];\n"
      << "\tint errors = 0;\n"
      << "\tint cycle  = 0;\n\n"
      << "\ttask step(input logic [N-1:0] x, input logic [N-1:0] y);\n"
      << "\t\t@(negedge clk);\n"
      << "\t\tif (cycle >= LAT) begin\n"
      << "\t\t\tif (out !== expected[LAT-1]) begin\n"
      << "\t\t\t\terrors++;\n"
      << "\t\t\t\t$display(\"" << fmt << "  <-- MISMATCH\",\n"
      << "\t\t\t\t         hist_a[LAT-1], hist_b[LAT-1], out, expected[LAT-1]);\n"
      << "\t\t\tend else begin\n"
      << "\t\t\t\t$display(\"" << fmt << "\",\n"
      << "\t\t\t\t         hist_a[LAT-1], hist_b[LAT-1], out, expected[LAT-1]);\n"
      << "\t\t\tend\n"
      << "\t\tend\n"
      << "\t\tfor (int k = LAT-1; k > 0; k--) begin\n"
      << "\t\t\texpected[k] = expected[k-1];\n"
      << "\t\t\thist_a[k]   = hist_a[k-1];\n"
      << "\t\t\thist_b[k]   = hist_b[k-1];\n"
      << "\t\tend\n"
      << "\t\texpected[0] = x * y;\n"
      << "\t\thist_a[0]   = x;\n"
      << "\t\thist_b[0]   = y;\n"
      << "\t\ta = x;\n"
      << "\t\tb = y;\n"
      << "\t\tcycle++;\n"
      << "\tendtask\n\n"
      << "\tinitial begin\n"
      << "\t\tfor (int k = 0; k < LAT; k++) begin\n"
      << "\t\t\texpected[k] = '0;\n"
      << "\t\t\thist_a[k]   = '0;\n"
      << "\t\t\thist_b[k]   = '0;\n"
      << "\t\tend\n"
      << "\t\ta = '0; b = '0;\n\n"
      << "\t\tstep('0, '0);\n"
      << "\t\tstep('1, '1);\n"
      << "\t\tstep('1, '0);\n"
      << "\t\tstep('0, '1);\n\n";

    if (N <= 8) {
        f << "\t\tfor (int i = 0; i < (1 << (2*N)); i++)\n"
          << "\t\t\tstep(i[2*N-1:N], i[N-1:0]);\n\n";
    } else {
        f << "\t\tfor (int i = 0; i < " << tests << "; i++)\n"
          << "\t\t\tstep(" << rnd << ", " << rnd << ");\n\n";
    }

    f << "\t\trepeat (LAT + 2) step('0, '0);\n\n"
      << "\t\tif (errors == 0) $display(\"PASS\");\n"
      << "\t\telse             $display(\"FAIL: errors = %0d\", errors);\n"
      << "\t\t$finish;\n"
      << "\tend\n"
      << "endmodule\n";
}