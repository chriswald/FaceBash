#ifndef __LOGIN_FIELD_H
#define __LOGIN_FIELD_H

#include <cstdlib>
#include <iostream>
#include <string>
#include <termios.h>
#include <unistd.h>

#include <readline/readline.h>

//#define WIN32
#ifdef WIN32
#include <windows.h>
#endif

using std::cin;
using std::cout;
using std::endl;
using std::string;

class LoginField
{
public:
   LoginField() {}
   ~LoginField() {}
   
   void readUser(string prompt = string("Username: "));
   void readPass(string prompt = string("Password: "));
   
   string user() {return username;}
   string pass() {return password;}
   
private:
   string username;
   string password;
   
   void SetStdinEcho(bool enable);
};

#endif //__LOGIN_FIELD_H
