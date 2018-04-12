#pragma once

#include <fstream>
#include <string>
#include <cstring>
#include <sstream>
#include <vector>
#include "exception.h"

using mips_program = ::std::vector<unsigned>;

::std::string getRegLabel(int index)
{
	static ::std::string labels[] = {
		"$0", "$at", "$v0", "$v1", "$a0", "$a1", "$a2", "$a3", 
		"$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7", 
		"$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7",
		"$t8", "$t9", "$k0", "$k1", "$gp", "$sp", "$fp", "$ra"
	};
	return labels[index];
}

::std::string getImm(int imm)
{
	::std::ostringstream os;
	os << imm;
	return os.str();
}

::std::string getAddr(int reg, int offset)
{
	return getImm(offset) + "(" + getRegLabel(reg) + ")";
}

mips_program load_mips_program(const ::std::string &file)
{
	// ifstream is(argv[i]);
	::std::ifstream is(file);
	::std::string l;
	mips_program prog;
	while (getline(is, l))
	{
		auto p = &l[0];
		while (*p && strchr(" \t", *p))
		{
			++p;
		}
		if (*p && *p != ';')
		{
			if (!strncmp(p, "memory_initialization_radix", sizeof("memory_initialization_radix") - 1))
			{
				//
			}
			else if (!strncmp(p, "memory_initialization_vector", sizeof("memory_initialization_vector") - 1))
			{
				//
			}
			else
			{
				while (*p)
				{
					auto q = p;
					while (*p && !strchr(",;", *p))
					{
						++p;
					}
					::std::string s(q, p);
					++p;
					while (*p && strchr(" \t", *p))
					{
						++p;
					}
					unsigned v;
					::std::istringstream iss(s);
					iss >> ::std::hex >> v;
					prog.push_back(v);
				}
			}
		}
	}
	return prog;
}

::std::string mips_disassembly(int code)
{
	union {
		struct {
			unsigned func: 6;
			unsigned shamt: 5;
			unsigned rd: 5;
			unsigned rt: 5;
			unsigned rs: 5;
			unsigned op: 6;
		} rtype;
		struct {
			unsigned imm: 16;
			unsigned rt: 5;
			unsigned rs: 5;
			unsigned op: 6;
		} itype;
		struct {
			unsigned addr: 26;
			unsigned op: 6;
		} jtype;
		int code;
	} decoder;
	decoder.code = code;
	switch (decoder.itype.op)
	{
	case 0x8: // addi
		return "addi " + getRegLabel(decoder.itype.rt) + ", " + 
			getRegLabel(decoder.itype.rs) + ", " +
			getImm(decoder.itype.imm);
	case 0xd: // ori
		return "ori " + getRegLabel(decoder.itype.rt) + ", " + 
			getRegLabel(decoder.itype.rs) + ", " +
			getImm(decoder.itype.imm);
	case 0x23: // lw
		return "lw " + getRegLabel(decoder.itype.rt) + ", " + 
			getAddr(decoder.itype.rs, decoder.itype.imm);
	case 0x2b: // sw
		return "sw " + getRegLabel(decoder.itype.rt) + ", " + 
			getAddr(decoder.itype.rs, decoder.itype.imm);
	case 0xa: // slti
		return "slti " + getRegLabel(decoder.itype.rt) + ", " + 
			getRegLabel(decoder.itype.rs) + ", " +
			getImm(decoder.itype.imm);
	case 0x4: // beq
		return "beq " + getRegLabel(decoder.itype.rs) + ", " + 
			getRegLabel(decoder.itype.rt) + ", " +
			getImm(decoder.itype.imm);
	case 0x5: // bne
		return "bne " + getRegLabel(decoder.itype.rs) + ", " + 
			getRegLabel(decoder.itype.rt) + ", " +
			getImm(decoder.itype.imm);
	case 0x2: // j
		return "j " + getImm(decoder.jtype.addr);
	case 0x3: // jal
		return "jal " + getImm(decoder.jtype.addr);
	case 0:
		switch (decoder.rtype.func)
		{
		case 0x20: // add
			return "add " + getRegLabel(decoder.rtype.rd) + ", " + 
				getRegLabel(decoder.rtype.rs) + ", " +
				getRegLabel(decoder.rtype.rt);
		case 0x22: // sub
			return "sub " + getRegLabel(decoder.rtype.rd) + ", " + 
				getRegLabel(decoder.rtype.rs) + ", " +
				getRegLabel(decoder.rtype.rt);
		case 0x24: // and
			return "and " + getRegLabel(decoder.rtype.rd) + ", " + 
				getRegLabel(decoder.rtype.rs) + ", " +
				getRegLabel(decoder.rtype.rt);
		case 0x25: // or
			return "or " + getRegLabel(decoder.rtype.rd) + ", " + 
				getRegLabel(decoder.rtype.rs) + ", " +
				getRegLabel(decoder.rtype.rt);
		case 0x27: // or
			return "nor " + getRegLabel(decoder.rtype.rd) + ", " + 
				getRegLabel(decoder.rtype.rs) + ", " +
				getRegLabel(decoder.rtype.rt);
		case 0x00: // sll 
			return "sll " + getRegLabel(decoder.rtype.rd) + ", " + 
				getRegLabel(decoder.rtype.rt) + ", " +
				getRegLabel(decoder.rtype.shamt);
		case 0x02: // srl
			return "srl " + getRegLabel(decoder.rtype.rd) + ", " + 
				getRegLabel(decoder.rtype.rt) + ", " +
				getRegLabel(decoder.rtype.shamt);
		case 0x2a: // slt
			return "slt " + getRegLabel(decoder.rtype.rd) + ", " + 
				getRegLabel(decoder.rtype.rs) + ", " +
				getRegLabel(decoder.rtype.rt);
		case 0x8: // jr
			return "jr " + getRegLabel(decoder.itype.rs);
		default:
			throwError("invalid instruction.");
		}
	default:
		throwError("invalid instruction.");
	}
}