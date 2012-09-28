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
  cURLpp::Easy request;
  cURLpp::Forms formParts;
  cURLpp::options::Url url(string("https://graph.facebook.com/me/feed"));
  string token = authToken();
  string message = prompt(string("Status: "));

  formParts.push_back(new cURLpp::FormParts::Content("access_token", token));
  formParts.push_back(new cURLpp::FormParts::Content("message", message));
  
  request.setOpt(url);
  request.setOpt(new cURLpp::Options::HttpPost(formParts));
  request.perform();
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

string ArgParse::prompt(string message)
{
  string tmp;
  cout << message;
  getline(cin, tmp);
  return tmp;
}

string ArgParse::authToken()
{
  string token;
  ifstream fin;
  fin.open("member27");
  if (fin.is_open())
    {
      fin >> token;
      fin.close();
      return token;
    }
  else
    {
      cerr << "Please Log In" << endl;
      Login();
      fin.open("member27");
      if (fin.is_open())
	{
	  fin >> token;
	  fin.close();
	  return token;
	}
      else
	{
	  cerr << "Could not log in." << endl;
	}
    }

  return NULL;
}
