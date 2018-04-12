#include <mipsa_options.h>
#include <new_parser/parser.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <mips_lang.h>

using namespace std;

void initEngine()
{
	using namespace mips;
	using namespace inst;
	rtype::add<rd, rs, rt>("add", 0x20);
	rtype::add<rd, rs, rt>("addu", 0x21);
	itype::add<rt, rs, imm>("addi", 0x8);
	itype::add<rt, rs, immu>("addiu", 0x9);
	rtype::add<rd, rs, rt>("and", 0x24);
	itype::add<rt, rs, imm>("andi", 0xc);
	rtype::add<rd, rs>("clo", 0x21, 0x1c);
	rtype::add<rd, rs>("clz", 0x20, 0x1c);
	rtype::add<rs, rt>("div", 0x1a);
	rtype::add<rs, rt>("divu", 0x1b);
	rtype::add<rs, rt>("mult", 0x18);
	rtype::add<rs, rt>("multu", 0x19);
	rtype::add<rd, rs, rt>("mul", 0x2, 0x1c);
	rtype::add<rs, rt>("madd", 0x0, 0x1c);
	rtype::add<rs, rt>("maddu", 0x1, 0x1c);
	rtype::add<rs, rt>("msub", 0x4, 0x1c);
	rtype::add<rs, rt>("msubu", 0x5, 0x1c);
	rtype::add<rd, rs, rt>("nor", 0x27);
	rtype::add<rd, rs, rt>("or", 0x25);
	itype::add<rt, rs, imm>("ori", 0xd);
	rtype::add<rd, rt, shamt>("sll", 0x0);
	rtype::add<rd, rt, rs>("sllv", 0x4);
	rtype::add<rd, rt, shamt>("sra", 0x3);
	rtype::add<rd, rt, rs>("srav", 0x7);
	rtype::add<rd, rt, shamt>("srl", 0x2);
	rtype::add<rd, rt, rs>("srlv", 0x6);
	rtype::add<rd, rs, rt>("sub", 0x22);
	rtype::add<rd, rs, rt>("subu", 0x23);
	rtype::add<rd, rs, rt>("xor", 0x26);
	itype::add<rt, rs, imm>("xori", 0xe);
	itype::add<rt, imm>("lui", 0xf);
	rtype::add<rd, rs, rt>("slt", 0x2a);
	rtype::add<rd, rs, rt>("sltu", 0x2b);
	itype::add<rt, rs, imm>("slti", 0xa);
	itype::add<rt, rs, immu>("sltiu", 0x2b);
	// rtype::add<rd, rs, rt>("bclf", 0x11);
	itype::add<rs, rt, label>("beq", 0x4);
	// rtype::add<rd, rs, rt>("bgez", 0x1);
	itype::add<rs, label>("bgtz", 0x7);
	itype::add<rs, label>("sub", 0x6);
	itype::add<rs, label>("bltz", 0x1);
	itype::add<rs, rt, label>("bne", 0x5);
	jtype::add<target>("j", 0x2);
	jtype::add<target>("jal", 0x3);
	rtype::add<rs, rd>("jalr", 0x9);
	rtype::add<rs>("jr", 0x8);
	rtype::add<rs, rt>("teq", 0x34);
	rtype::add<rs, rt>("tne", 0x36);
	rtype::add<rs, rt>("tge", 0x30);
	rtype::add<rs, rt>("tgeu", 0x31);
	rtype::add<rs, rt>("tlt", 0x32);
	rtype::add<rs, rt>("tltu", 0x33);
	// lb lbu
	itype::add<rt, addr>("lw", 0x23);
	//
	itype::add<rt, addr>("sw", 0x2b);
}

int main(int argc, char *argv[])
{
	option_parser options(argc, argv);
	initEngine();
	auto engine = mips::engine();
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
			ofstream(options.output_file_name) << engine.assembly(mips_code);
		}
	}
	catch (parser<mips::ast_type>::exception_type e)
	{
		raise(e.what());
	}
	// catch (...)
	// {
	// 	raise("Parse error.");
	// }
	return 0;
}