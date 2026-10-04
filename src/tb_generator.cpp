#include "tb_generator.hpp"

void generate_tb(int N, int latency, const std::string& dut) {
	int tests = 100000;

	if(N > 8)
		tests = std::max(1000, std::min(100000, 100000/N));
	
	std::ofstream f("tb_" + dut + ".sv");

	std::string rnd = "{";
	for (int i = 0; i < (N + 31) / 32; i++) rnd += (i ? ", $urandom" : "$urandom");
	rnd += "}";

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
	  << "\tint errors = 0;\n"
	  << "\tint cycle  = 0;\n\n"
	  << "\ttask step(input logic [N-1:0] x, input logic [N-1:0] y);\n"
	  << "\t\t@(negedge clk);\n"
	  << "\t\tif (cycle >= LAT && out !== expected[LAT-1]) begin\n"
	  << "\t\t\terrors++;\n"
	  << "\t\t\tif (errors <= 10)\n"
	  << "\t\t\t\t$display(\"ERROR t=%0t: out=%0h, expected=%0h\", $time, out, expected[LAT-1]);\n"
	  << "\t\tend\n"
	  << "\t\tfor (int k = LAT-1; k > 0; k--) expected[k] = expected[k-1];\n"
	  << "\t\texpected[0] = x * y;\n"
	  << "\t\ta = x;\n"
	  << "\t\tb = y;\n"
	  << "\t\tcycle++;\n"
	  << "\tendtask\n\n"
	  << "\tinitial begin\n"
	  << "\t\tfor (int k = 0; k < LAT; k++) expected[k] = '0;\n"
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