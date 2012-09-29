#include "ArgParse.h"

/*
 * Constructor:
 * Sets the count to the number of arguments and copies all arguments to to an
 * array of strings.
 */
ArgParse::ArgParse(int argc, char **argv)
{
  count = argc;
  arguments = new string[count];
  for (int i = 0; i < count; i ++)
    arguments[i] = string(argv[i]);
}

/*
 * Destructor:
 * Returns arguments array to the heap.
 */
ArgParse::~ArgParse()
{
  delete[] arguments;
}

/*
 * Parse Arguments:
 * Searches the provided arguments for meaningful inputs and calls corresponding
 * functions to handle heavy lifting.
 */
void ArgParse::ParseArgs()
{
  if      (argHas("-l") || argHas("--login"))
    Login();

  else if (argHas("-s") || argHas("--update_status"))
    UpdateStatus();

  else if (argHas("-n") || argHas("--show_news_feed"))
    ShowNewsFeed();
}

/*
 * Arguments Have:
 * Returns true if the argument list contains the specified argument, false
 * otherwise.
 */
bool ArgParse::argHas(string arg)
{
  for (int i = 0; i < count; i ++)
    {
      if (strcmp(arguments[i].c_str(), arg.c_str()) == 0)
	{
	  return true;
	}
    }
  return false;
}

/*
 * Log In:
 * Handles all responsibilities associated with retrieving a valid
 * authentification token. The user is prompted for their email and password,
 * then the process is forked in order to call a python script which handles the
 * logging in. The parent process waits for the script to exit, then runs a
 * check to see if the authentification token exists, informing the user whether
 * the login completed successfully or not based on the results of that check.
 */
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

/*
 * Update Status:
 * Handles all tasks associated with allowing a user to update his or her
 * status. Prompts the user for a status message, then polls for the
 * authentification token, then submits a POST to the Facebook Graph APIl
 */
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
  ostringstream os;
  cURLpp::Easy request;
  cURLpp::options::Url url(string("https://graph.facebook.com/me/statuses") + string("?access_token=") + authToken());
  request.setOpt(url);

  os << request;
  cout << os.str();
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
