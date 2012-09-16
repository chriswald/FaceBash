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

void LoginField::readUser(std::string prompt)
{
  std::cout << prompt.c_str();

  std::string s;
  getline(std::cin, s);

  LoginField::username = s;

  std::cout << std::flush;
}

void LoginField::readPass(std::string prompt)
{
  // Print the field prompt, then turn off echo
  std::cout << prompt.c_str();
  LoginField::SetStdinEcho(false);

  // Read input
  std::string s;
  getline(std::cin, s);

  // Store as password
  LoginField::password = s;

  // Turn echo back on and replace an endl that
  // was missed from getline() when echo was off
  LoginField::SetStdinEcho(true);
  std::cout << std::endl << std::flush;
}
