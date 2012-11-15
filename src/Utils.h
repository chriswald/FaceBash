#ifndef __UTILS_H
#define __UTILS_H

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
   static string prompt(string message);
   
private:
   static char* line_read;
};

#endif // __UTILS_H
