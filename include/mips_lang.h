#pragma once

#include <new_parser/parser.h>
#include <new_parser/variant/variant.h>
#include <initializer_list>
#include <map>
#include <string>
#include <sstream>
#include <type_traits>
#include <iomanip>

#define Expand() \
make_reflect<ast_type>([](ast_type &ast){\
	for (auto &e: ast) e.gen(); return value_type();\
})
#define NoReflect() \
make_reflect<ast_type>([](ast_type &ast){\
	return value_type();\
})
#define Print(x) \
(mips::protected_mips_lang__::get_machine_code().push_back(reinterpret_cast<unsigned&>(x)))

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

inline ::std::string get_reg_label(unsigned index)
{
	static ::std::string labels[] = {
		"$0", "$at", "$v0", "$v1", "$a0", "$a1", "$a2", "$a3", 
		"$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7", 
		"$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7",
		"$t8", "$t9", "$k0", "$k1", "$gp", "$sp", "$fp", "$ra"
	};
	return labels[index];
}

inline ::std::string get_imm(int imm)
{
	::std::ostringstream os; os << imm; return os.str();
}

inline ::std::string get_immu(unsigned imm)
{
	::std::ostringstream os; os << imm; return os.str();
}

inline ::std::string get_addr(int reg, int offset)
{
	return get_imm(offset) + "(" + get_reg_label(reg) + ")";
}

inline ::std::string get_label(int label)
{
	return get_imm(label);
}

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

template <typename ic>
union encoder
{
	constexpr encoder(unsigned code):
		code(code)
	{
	}

	constexpr unsigned value() const
	{
		return code >> ic::lowbit;
	}
private:
	struct { unsigned code: ic::field + ic::lowbit; };
};

}

template <typename ic, typename ...ics>
struct decode_inst
{
	static ::std::string gen(unsigned inst)
	{
		return decode_inst<ic>::gen(inst) + ", " + decode_inst<ics...>::gen(inst);
	}
};

template <typename ic>
struct decode_inst<ic>
{
	static ::std::string gen(unsigned inst)
	{
		return ic::gen_inst(inst);
	}
};

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

inline ::std::map<unsigned, variant<::std::function<::std::string(unsigned)>, 
		::std::map<unsigned, ::std::function<::std::string(unsigned)>>> >
	&get_map()
{
	static ::std::map<unsigned, variant<::std::function<::std::string(unsigned)>, 
		::std::map<unsigned, ::std::function<::std::string(unsigned)>>> > mm;
	return mm;
}

inline void on(unsigned op, unsigned func, const ::std::function<::std::string(unsigned)> &f)
{
	auto &mm = get_map();
	if (!mm.count(op))
	{
		mm[op] = ::std::map<unsigned, ::std::function<::std::string(unsigned)>>();
	}
	mm[op].get<::std::map<unsigned, ::std::function<::std::string(unsigned)>>>()[func]
		= f;
}

inline void on(unsigned op, const ::std::function<::std::string(unsigned)> &f)
{
	auto &mm = get_map();
	mm[op] = ::std::function<::std::string(unsigned)>(f);//.get<::std::function<::std::string(unsigned)>>()
		// = f;
}

inline ::std::string disassembly(unsigned inst)
{
	auto &mm = get_map();
	union {
		struct {
			unsigned func: 6;
			unsigned : 20;
			unsigned op:6;
		};
		unsigned value;
	} code;
	code.value = inst;
	::std::string result;
	if (mm.count(code.op))
	{
		mm[code.op].make_match
			<::std::function<::std::string(unsigned)>, 
				::std::map<unsigned, ::std::function<::std::string(unsigned)>>>
		(
			[&result, inst](::std::function<::std::string(unsigned)> &f)
			{
				result = f(inst);
			},
			[&result, inst, code](::std::map<unsigned, ::std::function<::std::string(unsigned)>> &mmm)
			{
				if (mmm.count(code.func))
				{
					result = mmm[code.func](inst);
				}
				else
				{
					std::ostringstream os; 
					os << ::std::setw(8) << ::std::setfill('0') << ::std::hex << inst;
					throw ::std::logic_error("unknown instruction: " + os.str());
				}
			}
		);
	}
	else
	{
		std::ostringstream os; 
		os << ::std::setw(8) << ::std::setfill('0') << ::std::hex << inst;
		throw ::std::logic_error("unknown instruction: " + os.str());
	}
	return result;
}

}

struct rs: protected_mips_inst__::reg<21>
{
	static unsigned gen_code(ast_type &ast, int index) { return 
		protected_mips_inst__::decoder<rs>(ast.term(index).get<int>()).value(); }
	static ::std::string gen_inst(unsigned code) { return
		get_reg_label(protected_mips_inst__::encoder<rs>(code).value()); }
};

struct rt: protected_mips_inst__::reg<16>
{
	static unsigned gen_code(ast_type &ast, int index) { return 
		protected_mips_inst__::decoder<rt>(ast.term(index).get<int>()).value(); }
	static ::std::string gen_inst(unsigned code) { return
		get_reg_label(protected_mips_inst__::encoder<rt>(code).value()); }
};

struct rd: protected_mips_inst__::reg<11>
{
	static unsigned gen_code(ast_type &ast, int index) { return 
		protected_mips_inst__::decoder<rd>(ast.term(index).get<int>()).value(); }
	static ::std::string gen_inst(unsigned code) { return
		get_reg_label(protected_mips_inst__::encoder<rd>(code).value()); }
};

struct shamt: protected_mips_inst__::imm<5, 6>
{
	static unsigned gen_code(ast_type &ast, int index) { return 
		protected_mips_inst__::decoder<shamt>(ast.term(index).get<int>()).value(); }
	static ::std::string gen_inst(unsigned code) { return
		get_immu(protected_mips_inst__::encoder<shamt>(code).value()); }
};

struct imm: protected_mips_inst__::imm<16, 0>
{
	static unsigned gen_code(ast_type &ast, int index) { return 
		protected_mips_inst__::decoder<imm>(ast.term(index).get<int>()).value(); }
	static ::std::string gen_inst(unsigned code) { return
		get_imm(protected_mips_inst__::encoder<imm>(code).value()); }
};

struct immu: protected_mips_inst__::imm<16, 0>
{
	static unsigned gen_code(ast_type &ast, int index) { return 
		protected_mips_inst__::decoder<immu>(ast.term(index).get<int>()).value(); }
	static ::std::string gen_inst(unsigned code) { return
		get_immu(protected_mips_inst__::encoder<immu>(code).value()); }
};

struct target: protected_mips_inst__::label<26, 0>
{
	static unsigned gen_code(ast_type &ast, int index) { return 
		protected_mips_inst__::decoder<target>(ast.term(index).get<int>()).value(); }
	static ::std::string gen_inst(unsigned code) { return
		get_label(protected_mips_inst__::encoder<target>(code).value()); }
};

struct label: protected_mips_inst__::label<16, 0>
{
	static unsigned gen_code(ast_type &ast, int index) { return 
		protected_mips_inst__::decoder<label>(ast.term(index).get<int>()).value(); }
	static ::std::string gen_inst(unsigned code) { return
		get_label(protected_mips_inst__::encoder<label>(code).value()); }
};

struct addr: protected_mips_inst__::addr<16, 0>
{
	static unsigned gen_code(ast_type &ast, int index) { return 
		protected_mips_inst__::decoder<addr>(ast.term(index).get<int>()).value() |
		protected_mips_inst__::decoder<rs>(ast.term(index + 2).get<int>()).value(); }
	static ::std::string gen_inst(unsigned code) { return
		get_addr(protected_mips_inst__::encoder<rs>(code).value(),
			protected_mips_inst__::encoder<addr>(code).value()); }
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

	inline ::std::vector<unsigned> &get_machine_code()
	{
		static ::std::vector<unsigned> machine_code;
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
	inst::protected_mips_inst__::on(op, func,
		[=](unsigned code) -> ::std::string {
			return inst + " " + inst::decode_inst<ics...>::gen(code);
		}
	);
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
void add(const ::std::string &inst, unsigned op)
{
	protected_mips_lang__::get_inst_lex().push_back(operator ""_t(inst.c_str(), inst.length()) = 
		operator ""_riw(inst.c_str(), inst.length()));
	protected_mips_lang__::get_inst_set() = protected_mips_lang__::get_inst_set() | 
		protected_mips_inst__::gen<ics...>(inst, op);
	inst::protected_mips_inst__::on(op, 
		[=](unsigned code) -> ::std::string {
			return inst + " " + inst::decode_inst<ics...>::gen(code);
		}
	);
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
void add(const ::std::string &inst, unsigned op)
{
	protected_mips_lang__::get_inst_lex().push_back(operator ""_t(inst.c_str(), inst.length()) = 
		operator ""_riw(inst.c_str(), inst.length()));
	protected_mips_lang__::get_inst_set() = protected_mips_lang__::get_inst_set() | 
		protected_mips_inst__::gen<ics...>(inst, op);
	inst::protected_mips_inst__::on(op, 
		[=](unsigned code) -> ::std::string {
			return inst + " " + inst::decode_inst<ics...>::gen(code);
		}
	);
}

}

class engine
{
public:
	::std::vector<unsigned> assembly(const ::std::string &s)
	{
		the_parser.parse(s.c_str());
		return protected_mips_lang__::get_machine_code();
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
