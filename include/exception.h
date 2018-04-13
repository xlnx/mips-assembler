#pragma once

#include <cstdlib>
#include <iostream>
#include <string>

inline void raise(const ::std::string &err)
{
	::std::cerr << "mipsa: " << err << ::std::endl;
	exit(1);
}
