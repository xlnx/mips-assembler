#include <mipsa_options.h>
#include <new_parser/parser.h>
#include <new_parser/variant/variant.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

using ast_type = ast<variant<unsigned, int>>;
// using ast_type = ast<long long>;
using value_type = typename ast_type::value_type;

int main(int argc, char *argv[])
{
	option_parser options(argc, argv);
	ostringstream machine_code;
	machine_code << hex;
	reflected_lexer<ast_type> lex
	(
		#include <mipsa_lexer.lang>
		// #include <calc_lexer.lang>
	);
	parser<ast_type> mips_parser
	(
		lex,
		#include <mipsa_parser.lang>
		// #include <calc_parser.lang>
	);
	ifstream fin(options.input_file_name);
	string mips_code, line;
	while (getline(fin, line))
	{
		mips_code += line + "\n";
	}
	try
	{
		if (options.disassembly)
		{
			raise("Disassembly not implemented.");
		}
		else
		{
			mips_parser.parse(mips_code.c_str());
		}
	}
	catch (parser<ast_type>::exception_type e)
	{
		raise(e.what());
	}
	catch (...)
	{
		raise("Parse error.");
	}
	string output_file_name;
	ofstream fout(output_file_name);
	fout << machine_code.str();
	return 0;
}