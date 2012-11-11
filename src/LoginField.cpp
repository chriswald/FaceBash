#include "LoginField.h"

void LoginField::SetStdinEcho(bool enable)
{
#ifdef WIN32
   HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
   DWORD mode;
   GetConsoleMode(hStdin, &mode);
   
   if (!enable)
      mode &= ~ENABLE_ECHO_INPUT;
   else
      mode |= ENABLE_ECHO_INPUT;
   
   SetConsoleMode(hStdin, mode);
   
#else
   struct termios tty;
   tcgetattr(STDIN_FILENO, &tty);
   if (!enable)
      tty.c_lflag &= ~ECHO;
   else
      tty.c_lflag |= ECHO;
   
   (void) tcsetattr(STDIN_FILENO, TCSANOW, &tty);
   
#endif
}

void LoginField::readUser(string prompt)
{
   char * tmp;
   tmp = readline(prompt.c_str());
   username = string(tmp);
   free(tmp);
}

void LoginField::readPass(std::string prompt)
{
   // Print the field prompt, then turn off echo
   cout << prompt.c_str();
   SetStdinEcho(false);
   
   // Read input and store as password
   string s;
   getline(cin, password);
   
   // Turn echo back on and replace an endl that
   // was missed from getline() when echo was off
   SetStdinEcho(true);
   cout << endl;
}
