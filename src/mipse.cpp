#include "disassembly.h"
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <iostream>
#include <cstring>
#include <string>
#include <vector>
#include "exception.h"

using namespace std;

void printHelp()
{
	auto help = ""\
	"Mipse version 0.1\n"\
	"-h/-help\n"\
	"\tshow this help message\n"\
	"";
	cerr << help << endl;
}

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		printHelp();
	}
	for (int i = 1; i != argc; ++i)
	{
		if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "-help"))
		{
			printHelp();
			return 0;
		}
	}
	vector<unsigned> prog;
	for (int i = 1; i != argc; ++i)
	{
		if (!strcmp(argv[i], "-v"))
		{
			//
		}
		else
		{
			prog = load_mips_program(argv[i]);
		}
	}
	for (auto is: prog)
	{
		// union {
		// 	int value;
		// 	struct {
		// 		unsigned 
		// 	} rtype;
		// 	struct {

		// 	} itype;
		// } inst(is);
		// cout << hex << is << ": ";
		auto inst = mips_disassembly(is);
		cout << inst << endl;
	}
	return 0;
}