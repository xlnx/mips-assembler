#pragma once

#include <exception.h>
#include <map>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <iomanip>
#include <functional>

namespace mips
{


inline ::std::function<::std::vector<unsigned>(const ::std::string&)>
	get_decoder(const ::std::string &name)
{
	static ::std::map<::std::string, ::std::function<::std::vector<unsigned>(const ::std::string&)>>
		decoders = {
			{
				"plain",
				[](const ::std::string &code) -> ::std::vector<unsigned>
				{
					::std::vector<unsigned> prog;
					::std::istringstream is(code);
					::std::string l;
					while (::std::getline(is, l))
					{
						unsigned inst;
						::std::istringstream iss(l);
						if (!(iss >> ::std::hex >> inst))
						{
							throw 1;
						}
						prog.push_back(inst);
					}
					return prog;
				}
			},
			{
				"coe",
				[](const ::std::string &code) -> ::std::vector<unsigned>
				{
					::std::vector<unsigned> prog;
					::std::istringstream is(code);
					::std::string l;
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
									if (!*p) 
									{
										throw 1;
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
			}
		};
	if (!decoders.count(name))
	{
		raise("No such decoder name: " + name);
	}
	return decoders[name];
}

inline ::std::function<::std::string(const ::std::vector<unsigned> &)>
	get_encoder(const ::std::string &name)
{
	static ::std::map<::std::string, ::std::function<::std::string(const ::std::vector<unsigned> &)>>
		encoders = {
			{
				"plain",
				[](const ::std::vector<unsigned> &code) -> ::std::string
				{
					::std::ostringstream os;
					for (auto inst: code) os << ::std::setw(8) << ::std::setfill('0') << ::std::hex << inst << "\n";
					return os.str();
				}
			},
			{
				"coe",
				[](const ::std::vector<unsigned> &code) -> ::std::string
				{
					::std::ostringstream os;
					os << "memory_initialization_radix=16;\n";
					os << "memory_initialization_vector=";
					for (int i = 0; i < code.size() - 1; ++i)
					{
						if (i % 10 == 0) os << "\n";
						os << ::std::setw(8) << ::std::setfill('0') << ::std::hex << code[i] << ", ";
					}
					if (!code.empty())
					{
						if ((code.size() - 1) % 10 == 0) os << "\n";
						os << ::std::setw(8) << ::std::setfill('0') << ::std::hex << code.back() << ";";
					}
					return os.str();
				}
			}
		};
	if (!encoders.count(name))
	{
		raise("No such encoder name: " + name);
	}
	return encoders[name];
}

}