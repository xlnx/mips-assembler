#pragma once

#include <cstdlib>
#include <iostream>
#include <string>

inline void throwError(::std::string error)
{
	::std::cerr << "mipse: " << error << ::std::endl;
	exit(1);
}
