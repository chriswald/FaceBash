#include "Utils.h"

/*
 * Prompt:
 * Displays a prompt to the user and waits for input, returned as a
 * string.
 */
string Utils::prompt(string message)
{
  string tmp;
  cout << message;
  getline(cin, tmp);
  return tmp;
}
