#pragma once

#include <mips_util.h>
#include <mips_asm.h>
#include <initializer_list>
#include <map>
#include <string>

#define Expand() \
make_reflect<ast_type>([](ast_type &ast){\
	for (auto &e: ast) e.gen(); return value_type();\
})
#define NoReflect() \
make_reflect<ast_type>([](ast_type &ast){\
	return value_type();\
})

namespace mips
{

class engine
{
public:
	::std::vector<unsigned> assembly(const ::std::string &s)
	{
		constexpr int origin = 0;
		protected_mips_lang__::get_inst_addr() = origin;
		the_parser.parse(s.c_str());
		::std::vector<unsigned> res;
		for (auto &e: protected_mips_lang__::get_machine_code())
		{
			res.push_back(e.reinvoke());
		}
		return res;//protected_mips_lang__::get_machine_code();
	}
	::std::string disassembly(const ::std::vector<unsigned> &s)
	{
		::std::string res;
		for (auto inst: s)
		{
			res += inst::protected_mips_inst__::disassembly(inst) + "\n";
		}
		return res;
	}
private:
	reflected_lexer<ast_type> lex = reflected_lexer<ast_type>(
		// #include <mipsa_lexer.lang>
		inst::protected_mips_inst__::cat(
			inst::protected_mips_inst__::cat(
				"label"_t = "[A-Za-z_]\\w*"_rw
					>> lexer_reflect<ast_type>([](const ::std::string &src)->value_type{
						return ::std::string(src);
					}),
				protected_mips_lang__::get_inst_lex()
			),
			lexer_list_type{
				"reg"_t = "\\$(0|at|v[01]|a[0-4]|t[0-9]|s[0-7]|k[01]|gp|sp|fp|ra)"_rw
					>> lexer_reflect<ast_type>([](const ::std::string &src)->value_type{
						switch (src[1])
						{
						case '0': return 0;
						case 'a':
							switch (src[2])
							{
							case 't': return 1;
							default: return 4 + src[2] - '0';
							}
						case 'v': return 2 + src[2] - '0';
						case 't':
							switch (src[2])
							{
							case '8': case '9': return 24 + src[2] - '0';
							default: return 8 + src[2] - '0';
							}
						case 's':
							switch (src[2])
							{
							case 'p': return 29;
							default: return 16 + src[2] - '0';
							}
						case 'k': return 26 + src[2] - '0';
						case 'g': return 28;
						case 'f': return 30;
						case 'r': return 31;
						default: throw exception("Unexpected register value.");
						}
					}),

				"imm"_t = "(?:0[Xx][0-9A-Fa-f]+|0[0-7]*|[1-9][0-9]*)"_rw
					>> lexer_reflect<ast_type>([](const ::std::string &src)->value_type{
						if (src[0] == '0')
						{
							if (src.length() > 1 && (src[1] == 'X' || src[1] == 'x'))
							{
								::std::istringstream is(src.substr(2));
								int x; is >> ::std::hex >> x; return x;
							}
							else
							{
								::std::istringstream is(src.substr(1));
								int x = 0; is >> ::std::oct >> x; return x;
							}
						}
						else
						{
							::std::istringstream is(src);
							int x; is >> ::std::dec >> x; return x;
						}
					}),

				lexer_elem_type(":"_t = ":"_r),
				lexer_elem_type(","_t = ","_r),
				lexer_elem_type("("_t),
				lexer_elem_type(")"_t)
			}
		)
	);
	parser<ast_type> the_parser = parser<ast_type>(lex,
		"start"_p = 
			"Lines"_p >> Expand(),
		"Lines"_p = 
			"Line"_p + "Lines"_p >> Expand()
			|""_t >> NoReflect(),
		"Line"_p = 
			"Instruction"_p
				>> make_reflect<ast_type>([](ast_type &ast)->value_type{
					ast[0].gen(); protected_mips_lang__::get_inst_addr() ++;
					return value_type();
				})
			|"label"_t + ":"_t + "Instruction"_p
				>> make_reflect<ast_type>([](ast_type &ast)->value_type{
					auto &elem = ast.term(0).get<::std::string>();
					protected_mips_lang__::get_label_map()[elem] = 
						protected_mips_lang__::get_inst_addr();
					ast[0].gen(); protected_mips_lang__::get_inst_addr() ++;
					return value_type();
				}),
		"Instruction"_p = 
			protected_mips_lang__::get_inst_set()
	);
};

}
