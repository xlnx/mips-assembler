#pragma once

#include <exception.h>
#include <string>
#include <iostream>
#include <cstring>
#include <cstdlib>

struct option_parser
{
	option_parser(int argc, char *argv[])
	{
		for (int i = 1; i != argc; ++i)
		{
			if (!strcmp(argv[i], "-o"))
			{
				if (output_file_name != "")
				{
					raise("Duplicate output file.");
				}
				if (++i == argc)
				{
					raise("Unspecified output file.");
				}
				output_file_name = argv[i];
			}
			else if (!strcmp(argv[i], "-d"))
			{
				disassembly = true;
			}
			else if (!strcmp(argv[i], "-f"))
			{
				if (++i == argc)
				{
					raise("No format specified.");
				}
				format = argv[i];
			}
			else
			{
				if (input_file_name != "")
				{
					raise("Duplicate input file.");
				}
				input_file_name = argv[i];
			}
		}
		if (input_file_name == "")
		{
			raise("No input file.");
		}
		auto l = input_file_name.length();
		for (; l > 0; --l)
		{
			if (input_file_name[l] == '.') break;
		}
		if (output_file_name == "")
		{
			output_file_name = input_file_name.substr(0, l) + 
				(disassembly ? ".mips" : ".eco");
		}
	}
	
	::std::string input_file_name;
	::std::string output_file_name;
	::std::string format = "plain";
	bool disassembly = false;
};