#pragma once

#include <new_parser/parser.h>
#include <new_parser/variant/variant.h>
#include <initializer_list>
#include <string>
#include <sstream>
#include <type_traits>

#define Expand() \
make_reflect<ast_type>([](ast_type &ast){\
	for (auto &e: ast) e.gen(); return value_type();\
})
#define NoReflect() \
make_reflect<ast_type>([](ast_type &ast){\
	return value_type();\
})
#define Print(x) \
(mips::protected_mips_lang__::get_machine_code() << ::std::setw(8) << \
	::std::setfill('0') << reinterpret_cast<int&>(x) << ::std::endl)

namespace mips
{

using ast_type = ast<variant<unsigned, int>>;
// using ast_type = ast<long long>;
using value_type = typename ast_type::value_type;

using list_type = initializer<element>;

using rule_type = parser_rule<ast_type>;

using lexer_elem_type = reflected_lexer_init_element<ast_type, char>;

using lexer_list_type = initializer<lexer_elem_type>;

namespace inst
{

namespace protected_mips_inst__
{

struct base
{
};

template <int lowbitNum>
struct reg: base
{
	static constexpr int field = 5;
	static constexpr int lowbit = lowbitNum;
	static list_type token() { return "reg"_t; }
};

// template <int fieldNum, int lowbitNum>
// struct immu: base
// {
// 	static constexpr int field = fieldNum;
// 	static constexpr int lowbit = lowbitNum;
// 	static list_type token() { return "immu"_t; }
// };

template <int fieldNum, int lowbitNum>
struct imm: base
{
	static constexpr int field = fieldNum;
	static constexpr int lowbit = lowbitNum;
	static list_type token() { return "imm"_t; }
};

template <int fieldNum, int lowbitNum>
struct label: base
{
	static constexpr int field = fieldNum;
	static constexpr int lowbit = lowbitNum;
	static list_type token() { return "label"_t; }
};

template <int fieldNum, int lowbitNum>
struct addr: base
{
	static constexpr int field = fieldNum;
	static constexpr int lowbit = lowbitNum;
	static list_type token() { return "imm"_t + "("_t + "reg"_t + ")"_t; }
};

template <typename ic>
union decoder
{
	constexpr decoder(int code):
		code(code)
	{
	}

	constexpr unsigned value() const
	{
		return code << ic::lowbit;
	}
private:
	struct { unsigned code: ic::field; };
};

}

namespace protected_mips_inst__
{

template <typename ic, typename ...ics>
struct gen_code
{
	static unsigned gen(ast_type &ast, int index) { 
		auto r = ic::gen_code(ast, index);
		return r | gen_code<ics...>::gen(ast, index + ic::token().size() + 1); }
};

template <typename ic>
struct gen_code<ic>
{
	static unsigned gen(ast_type &ast, int index) { return ic::gen_code(ast, index); }
};

// template <typename element>
inline list_type cat(const element &elem, const list_type &list)
{
	list_type new_list = { elem }; 
	for (auto &e: list) new_list.push_back(e);
	return new_list;
}

inline list_type cat(const list_type &elem, const list_type &list)
{
	list_type new_list = elem;
	for (auto &e: list) new_list.push_back(e);
	return new_list;
}

inline lexer_list_type cat(const lexer_elem_type &elem, const lexer_list_type &list)
{
	lexer_list_type new_list = list; new_list.push_back(elem);
	return new_list;
}

inline lexer_list_type cat(const lexer_list_type &elem, const lexer_list_type &list)
{
	lexer_list_type new_list = list;
	for (auto &e: elem) new_list.push_back(e);
	return new_list;
}

template <typename ic, typename ...ics>
struct gen_inst
{
	static list_type gen() { return cat(ic::token(), cat(","_t, gen_inst<ics...>::gen())); }
};

template <typename ic>
struct gen_inst<ic>
{
	static list_type gen() { return ic::token(); }
};

}

struct rs: protected_mips_inst__::reg<21>
{
	static unsigned gen_code(ast_type &ast, int index) { return 
		protected_mips_inst__::decoder<rs>(ast.term(index).get<int>()).value(); }
};

struct rt: protected_mips_inst__::reg<16>
{
	static unsigned gen_code(ast_type &ast, int index) { return 
		protected_mips_inst__::decoder<rt>(ast.term(index).get<int>()).value(); }
};

struct rd: protected_mips_inst__::reg<11>
{
	static unsigned gen_code(ast_type &ast, int index) { return 
		protected_mips_inst__::decoder<rd>(ast.term(index).get<int>()).value(); }
};

struct shamt: protected_mips_inst__::imm<5, 6>
{
	static unsigned gen_code(ast_type &ast, int index) { return 
		protected_mips_inst__::decoder<shamt>(ast.term(index).get<int>()).value(); }
};

struct imm: protected_mips_inst__::imm<16, 0>
{
	static unsigned gen_code(ast_type &ast, int index) { return 
		protected_mips_inst__::decoder<imm>(ast.term(index).get<int>()).value(); }
};

struct immu: protected_mips_inst__::imm<16, 0>
{
	static unsigned gen_code(ast_type &ast, int index) { return 
		protected_mips_inst__::decoder<immu>(ast.term(index).get<int>()).value(); }
};

struct target: protected_mips_inst__::label<26, 0>
{
	static unsigned gen_code(ast_type &ast, int index) { return 
		protected_mips_inst__::decoder<target>(ast.term(index).get<int>()).value(); }
};

struct label: protected_mips_inst__::label<16, 0>
{
	static unsigned gen_code(ast_type &ast, int index) { return 
		protected_mips_inst__::decoder<label>(ast.term(index).get<int>()).value(); }
};

struct addr: protected_mips_inst__::addr<16, 0>
{
	static unsigned gen_code(ast_type &ast, int index) { return 
		protected_mips_inst__::decoder<addr>(ast.term(index).get<int>()).value() |
		protected_mips_inst__::decoder<rs>(ast.term(index + 2).get<int>()).value(); }
};

struct func: protected_mips_inst__::imm<6, 0>
{
};

struct op: protected_mips_inst__::imm<6, 26>
{
};

}

namespace protected_mips_lang__
{
	inline lexer_initializer &get_inst_lex()
	{
		static lexer_initializer lex;
		return lex;
	}

	inline parser_initializer<ast_type> &get_inst_set()
	{
		static parser_initializer<ast_type> set;
		return set;
	}

	inline ::std::ostringstream &get_machine_code()
	{
		static ::std::ostringstream machine_code;
		return machine_code;
	}
}

namespace rtype
{

namespace protected_mips_inst__
{

template <typename ...ics>
rule_type gen(const ::std::string &inst, unsigned func, unsigned op = 0)
{
	return parser_rule<ast_type>(
		inst::protected_mips_inst__::cat(operator ""_t(inst.c_str(), inst.length()),
			inst::protected_mips_inst__::gen_inst<ics...>::gen()),
		make_reflect<ast_type>(
			[=](ast_type &ast) -> value_type{
				auto inst = inst::protected_mips_inst__::gen_code<ics...>::gen(ast, 1);
				inst = inst | inst::protected_mips_inst__::decoder<inst::func>(func).value() |
					inst::protected_mips_inst__::decoder<inst::op>(op).value();
				Print(inst);
			}
		)
	);
}

}

template <typename ...ics>
void add(const ::std::string &inst, unsigned func, unsigned op = 0)
{
	protected_mips_lang__::get_inst_lex().push_back(operator ""_t(inst.c_str(), inst.length()) = 
		operator ""_riw(inst.c_str(), inst.length()));
	protected_mips_lang__::get_inst_set() = protected_mips_lang__::get_inst_set() | 
		protected_mips_inst__::gen<ics...>(inst, func, op);
}

}

namespace itype
{

namespace protected_mips_inst__
{

template <typename ...ics>
rule_type gen(const ::std::string &inst, unsigned op)
{
	return parser_rule<ast_type>(
		inst::protected_mips_inst__::cat(operator ""_t(inst.c_str(), inst.length()),
			inst::protected_mips_inst__::gen_inst<ics...>::gen()),
		make_reflect<ast_type>(
			[=](ast_type &ast) -> value_type{
				auto inst = inst::protected_mips_inst__::gen_code<ics...>::gen(ast, 1);
				inst = inst | inst::protected_mips_inst__::decoder<inst::op>(op).value();
				Print(inst);
			}
		)
	);
}

}

template <typename ...ics>
void add(const ::std::string &inst, unsigned func)
{
	protected_mips_lang__::get_inst_lex().push_back(operator ""_t(inst.c_str(), inst.length()) = 
		operator ""_riw(inst.c_str(), inst.length()));
	protected_mips_lang__::get_inst_set() = protected_mips_lang__::get_inst_set() | 
		protected_mips_inst__::gen<ics...>(inst, func);
}

}

namespace jtype
{

namespace protected_mips_inst__
{

template <typename ...ics>
rule_type gen(const ::std::string &inst, unsigned op)
{
	return parser_rule<ast_type>(
		inst::protected_mips_inst__::cat(operator ""_t(inst.c_str(), inst.length()),
			inst::protected_mips_inst__::gen_inst<ics...>::gen()),
		make_reflect<ast_type>(
			[=](ast_type &ast) -> value_type{
				auto inst = inst::protected_mips_inst__::gen_code<ics...>::gen(ast, 1);
				inst = inst | inst::protected_mips_inst__::decoder<inst::op>(op).value();
				Print(inst);
			}
		)
	);
}

}

template <typename ...ics>
void add(const ::std::string &inst, unsigned func)
{
	protected_mips_lang__::get_inst_lex().push_back(operator ""_t(inst.c_str(), inst.length()) = 
		operator ""_riw(inst.c_str(), inst.length()));
	protected_mips_lang__::get_inst_set() = protected_mips_lang__::get_inst_set() | 
		protected_mips_inst__::gen<ics...>(inst, func);
}

}

class engine
{
public:
	::std::string assembly(const ::std::string &s)
	{
		protected_mips_lang__::get_machine_code() = ::std::ostringstream();
		protected_mips_lang__::get_machine_code() << ::std::hex;
		the_parser.parse(s.c_str());
		return protected_mips_lang__::get_machine_code().str();
	}
private:
	reflected_lexer<ast_type> lex = reflected_lexer<ast_type>(
		// #include <mipsa_lexer.lang>
		inst::protected_mips_inst__::cat(
			inst::protected_mips_inst__::cat(
				"label"_t = "[A-Za-z_]\\w*"_rw
					>> lexer_reflect<ast_type>([](const ::std::string &src)->value_type{
						return int(::std::stoi(src.substr(1)));
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
							default: return 16 + src[3] - '0';
							}
						case 'k': return 26 + src[2] - '0';
						case 'g': return 28;
						case 'f': return 30;
						case 'r': return 31;
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
			"Instruction"_p + "Lines"_p >> Expand()
			|""_t >> NoReflect(),
		"Line"_p = 
			"LineBody"_p >> Expand(),
		"LineLabel"_p = 
			"label"_t + ":"_t
				>> make_reflect<ast_type>([](ast_type &ast)->value_type{
					//::std::cout << ast.term(0) << ::std::endl;
				})
			|""_t
				>> NoReflect(),
		"LineBody"_p = 
			//"Directive"_p >> Expand()
			"Instruction"_p >> Expand(),
		//"Directive"_p =
		//	""_t,
		"Instruction"_p = 
			protected_mips_lang__::get_inst_set()
	);
};

inline parser<ast_type> gen_parser()
{
	
};

}
