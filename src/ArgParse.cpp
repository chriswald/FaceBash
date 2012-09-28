#include "ArgParse.h"

ArgParse::ArgParse(int argc, char **argv)
{
  count = argc;
  argument = new string[count];
  for (int i = 0; i < count; i ++)
    argument[i] = string(argv[i]);
}

ArgParse::~ArgParse()
{}

void ArgParse::ParseArgs()
{
  if      (argHas("-l") || argHas("--login"))
    Login();

  else if (argHas("-s") || argHas("--update_status"))
    UpdateStatus();
}

bool ArgParse::argHas(string arg)
{
  for (int i = 0; i < count; i ++)
    {
      if (strcmp(argument[i].c_str(), arg.c_str()) == 0)
	{
	  return true;
	}
    }
  return false;
}

void ArgParse::Login()
{
  LoginField login = LoginField();
  login.readUser("Email: ");
  login.readPass();
  
  int status;
  pid_t pid = fork();
  
  if (pid < 0)
    {
      perror("fork error");
    }
  else if (pid == 0)
    {
      execl("/usr/bin/python2.7", "/usr/bin/python2.7", "scripts/login.py", login.user().c_str(), login.pass().c_str(), (char *) 0);
    }
  else
    {
      if ((pid = wait(&status)) == -1)
	perror("wait error");
    }
  
  std::ifstream member27;
  member27.open("member27");
  if (member27.is_open())
    {
      cout << "Logged In." << endl;
      member27.close();
    }
  else
    {
      cerr << "Unable to login." << endl;
    }
}

void ArgParse::UpdateStatus()
{
  
}

void ArgParse::ShowNewsFeed()
{

}

void ArgParse::AboutFriend()
{

}

void ArgParse::ShowUpcomingBirthdays()
{

}

void ArgParse::AboutMe()
{

}

string ArgParse::prompt(string message, ostream os, istream is)
{
  string tmp;
  os << message;
  is >> tmp;
  return tmp;
}
