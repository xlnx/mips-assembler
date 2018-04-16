#pragma once

#include <new_parser/parser.h>
#include <new_parser/variant/variant.h>
#include <functional>
#include <exception>
#include <vector>

namespace mips

{

struct exception: public ::std::logic_error
{
	exception(const ::std::string &msg):
		::std::logic_error(msg)
	{
	}
};

using ast_type = ast<variant<unsigned, int, ::std::string>>;
// using ast_type = ast<long long>;
using value_type = typename ast_type::value_type;

struct instruction
{
	instruction(unsigned val);
	instruction(const ::std::function<unsigned()> &f);

	unsigned reinvoke();
	instruction operator | (const instruction &other) const
	{
		if (inst_pos != other.inst_pos)
		{
			throw exception("Invalid operation.");
		}
		else
		{
			instruction i(*this);
			i.inst |= other.inst;
			for (auto &f: other.callback)
			{
				i.callback.push_back(f);
			}
			return i;
		}
	}
	instruction operator | (unsigned other)
	{
		instruction i(*this);
		i.inst |= other;
		return i;
	}
private:
	unsigned inst_pos;
	unsigned inst = 0;
	::std::vector<::std::function<unsigned()>> callback;
};

using list_type = initializer<element>;

using rule_type = parser_rule<ast_type>;

using lexer_elem_type = reflected_lexer_init_element<ast_type, char>;

using lexer_list_type = initializer<lexer_elem_type>;

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

	inline ::std::vector<instruction> &get_machine_code()
	{
		static ::std::vector<instruction> machine_code;
		return machine_code;
	}

	inline unsigned &get_inst_addr()
	{
		static unsigned addr = 0;
		return addr;
	}

	inline ::std::map<::std::string, unsigned> &get_label_map()
	{
		static ::std::map<::std::string, unsigned> m;
		return m;
	}
}

instruction::instruction(unsigned val):
	inst(val), inst_pos(protected_mips_lang__::get_inst_addr())
{
}

instruction::instruction(
		const ::std::function<unsigned()> &f):
	inst(0), inst_pos(protected_mips_lang__::get_inst_addr()), callback({f})
{
}

inline unsigned instruction::reinvoke()
{
	protected_mips_lang__::get_inst_addr() = inst_pos;
	auto inst = this->inst;
	for (auto &f: callback)
	{
		inst |= f();
	}
	return inst;
}

}