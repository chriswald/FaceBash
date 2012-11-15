#include "Utils.h"

/*
 * Prompt:
 * Displays a prompt to the user and waits for input, returned as a
 * string.
 */
string Utils::prompt(string message)
{
   char * tmp;
   tmp = readline(message.c_str());
   string ret (tmp);
   free(tmp);
   return ret;
}
