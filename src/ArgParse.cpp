#include "ArgParse.h"

/*
 * Constructor:
 * Sets the count to the number of arguments and copies all arguments
 * to to an array of strings. Does not copy the first argument, the
 * program name.
 */
ArgParse::ArgParse(int argc, char **argv)
{
  count = argc-1;
  arguments = new string[count];
  for (int i = 0; i < count; i ++)
    arguments[i] = string(argv[i+1]);
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
 * Searches the provided arguments for meaningful inputs and calls
 * corresponding functions to handle heavy lifting. Displays help text
 * if no arguments were passed.
 */
void ArgParse::ParseArgs()
{
  if (count == 0)
    {
      ShowHelpText();
      return;
    }

  if      (argHas("-h") || argHas("--help"))
    ShowHelpText();

  else if (argHas("-l") || argHas("--login"))
    Login();

  else if (argHas("-n") || argHas("--show_news_feed"))
    ShowNewsFeed();

  else if (argHas("-s") || argHas("--update_status"))
    UpdateStatus();

  else if (argHas("-v") || argHas("--version"))
    ShowVersion();
}

/*
 * Arguments Have:
 * Returns true if the argument list contains the specified argument,
 * false otherwise.
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
 * authentification token. The user is prompted for their email and
 * password, then the process is forked in order to call a python
 * script which handles the logging in. The parent process waits for
 * the script to exit, then runs a check to see if the
 * authentification token exists, informing the user whether the login
 * completed successfully or not based on the results of that check.
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
 * Handles all tasks associated with allowing a user to update his or
 * her status. Prompts the user for a status message, then polls for
 * the authentification token, then submits a POST to the Facebook
 * Graph API.
 */
void ArgParse::UpdateStatus()
{
  ostringstream os;
  cURLpp::Easy request;
  cURLpp::Forms formParts;
  cURLpp::options::Url url(string("https://graph.facebook.com/me/feed"));
  string token = authToken();
  string message = prompt(string("Status: "));

  formParts.push_back(new cURLpp::FormParts::Content("access_token", token));
  formParts.push_back(new cURLpp::FormParts::Content("message", message));
  
  request.setOpt(url);
  request.setOpt(new cURLpp::Options::HttpPost(formParts));
  os << request;
}

/*
 * Show News Feed:
 * Shows the user's news feed. Filters out all posts that are not only
 * text so that posts that don't make sense will be filtered out.
 */
void ArgParse::ShowNewsFeed()
{
  // Get the JSON from Facebook
  ostringstream os;
  cURLpp::Easy request;
  cURLpp::options::Url url(string("https://graph.facebook.com/me/home") + string("?access_token=") + authToken());
  request.setOpt(url);

  os << request;
  
  // Parse the JSON
  Json::Value root;
  Json::Reader reader;

  bool parsingSuccessful = reader.parse(os.str(), root);
  if (!parsingSuccessful)
    {
      cerr << "Failed to parse the document." << endl;
      return;
    }

  cout << root["error"]["message"].asString() << endl;

  const Json::Value posts = root["data"];
  for (unsigned int i = 0; i < posts.size(); i ++)
    {
      string content_type = posts[i]["type"].asString();
      if (strcmp(content_type.c_str(), "status") == 0)
	{
	  string posted_by = posts[i]["from"]["name"].asString();
	  string message = posts[i]["message"].asString();
	  if (strcmp(message.c_str(), "") != 0)
	    {
	      cout << posted_by << endl;
	      cout << "   " << posts[i]["message"].asString() << endl << endl;
	    }
	}
    }
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

/*
 * Show Help Text:
 * Displays a list of all accepted commands and their functions.
 */
void ArgParse::ShowHelpText()
{
  ShowVersion();
  cout                                                                  << endl;
  cout << "Usage: bin/Facebash [options]"                               << endl;
  cout                                                                  << endl;
  cout << "Options:"                                                    << endl;
  cout << "   -h, --help            Shows this message"                 << endl;
  cout << "   -l, --login           Logs a user into Facebook"          << endl;
  cout << "   -n, --show_news_feed  Shows the current user's news feed" << endl;
  cout << "   -s, --update_status   Updates the current user's status"  << endl;
  cout << "   -v, --version         Displays version information"       << endl;
  cout                                                                  << endl;
}

/*
 * Show Version Information:
 * Displays the program's current version and copyright information.
 */
void ArgParse::ShowVersion()
{
  cout << "F.acebash v" << version                 << endl;
  cout << "Copyright (c) 2012 Christopher J. Wald" << endl;
}

/*
 * Prompt:
 * Displays a prompt to the user and waits for input, returned as a
 * string.
 */
string ArgParse::prompt(string message)
{
  string tmp;
  cout << message;
  getline(cin, tmp);
  return tmp;
}

/*
 * Get Auth Token:
 * Attempts to retrieve the auth token. On success the auth token is
 * returned. On failure the user is prompted. The token is again
 * polled for. On success the token is returned. On failure a message
 * is displayed and NULL is returned.
 */
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
