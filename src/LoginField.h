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
   
   void readUser(std::string prompt = std::string("Username: "));
   void readPass(std::string prompt = std::string("Password: "));
   
   std::string user() {return username;}
   std::string pass() {return password;}
   
private:
   std::string username;
   std::string password;
   
   void SetStdinEcho(bool);
};

#endif //__LOGIN_FIELD_H
