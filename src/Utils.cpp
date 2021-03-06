#include "Utils.hpp"

/*
 * Prompt:
 * Displays a prompt to the user and waits for input, returned as a
 * string.
 */
string Utils::prompt(const string & message)
{
   char * tmp;
   tmp = readline(message.c_str());
   string ret (tmp);
   free(tmp);
   return ret;
}

string Utils::authFile()
{
   string homedir = string(getenv("HOME")) + string("/.facebash/");
   string filename = homedir + "member27";
   return filename;
}
