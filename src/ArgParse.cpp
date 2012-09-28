#include "ArgParse.h"

ArgParse::ArgParse(int argc, char *argv[])
{
  count = argc;
  args = argv;
}

ArgParse::~ArgParse()
{
  for (int i = 0; i < count; i ++)
    {
      delete[] args[i];
    }
}

void ArgParse::parseArgs()
{
  if      (argvHas("-l") || argvHas("--login"))
    Login();

  else if (argvHas("-s") || argvHas("--post_status"))
    UpdateStatus();

  else if (argvHas("-n") || argvHas("--news_feed"))
    ShowNewsFeed();

  else if (argvHas("-a") || argvHas("--about_friend"))
    AboutFriend();

  else if (argvHas("-b") || argvHas("--birthdays"))
    ShowUpcomingBirthdays();

  else if (                 argvHas("--about_me"))
    AboutMe();
}


bool ArgParse::argvHas(string arg)
{
  for (int i = 0; i < count; i ++)
    {
      if (strcmp(arg.c_str(), args[i]) == 0)
	return true;
    }

  return false;
}

string ArgParse::prompt(string message, ostream os, istream is)
{
  string reply;

  os << message;
  is >> reply;
  os << endl;

  return reply;
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
  member27.open("/home/pi/.facebash/member27");
  if (member27.is_open())
    {
      while (member27.good())
	{
	  std::string line;
	  getline (member27, line);
	  std::cout << line << std::endl;
	}
      member27.close();
    }
  else
    {
      std::cerr << "Unable to login." << std::endl;
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
