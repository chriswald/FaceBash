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
