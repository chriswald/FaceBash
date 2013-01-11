#ifndef __UTILS_HPP
#define __UTILS_HPP

#include <cstdlib>
#include <iostream>
#include <string>

#include <readline/readline.h>

using std::cin;
using std::cout;
using std::endl;
using std::string;

class Utils
{
public:
   static string prompt(const string & message);
   static string authFile();
};

#endif // __UTILS_HPP
