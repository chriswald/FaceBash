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

  else if (                argHas("--logout"))
    Logout();

  else if (argHas("-n") || argHas("--show_news_feed"))
    ShowNewsFeed();

  else if (argHas("-s") || argHas("--update_status"))
    UpdateStatus();

  else if (argHas("-v") || argHas("--version"))
    ShowVersion();

  else
    ShowHelpText();
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
      string content;
      member27 >> content;

      if (strlen(content.c_str()) > 1)
	{
	  cout << "Logged In." << endl;
	}
      else
	{
	  if      (strcmp(content.c_str(), "1") == 0)
	    {
	      cout << "Invalid Email or Password." << endl;
	    }
	  else if (strcmp(content.c_str(), "2") == 0)
	    {
	      cout << "User denied permission to F.aceBash." << endl;
	    }
	  else
	    {
	      cout << "An unknown error occured." << endl;
	    }
	}
    }
  else
    {
      cerr << "Unable to login." << endl;
    }

  member27.close();
}

/*
 * Log Out:
 * Really just deletes the file containing the authentication token.
 */
void ArgParse::Logout()
{
  remove("member27");
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
  string who;
  if (count > 1)
    {
      who = getFriendID(arguments[1]);
      if (strcmp(who.c_str(), "\0") == 0)
	{
	  return;
	}
    }
  else
    {
      who = "me";
    }

  ostringstream os;
  cURLpp::Easy request;
  cURLpp::Forms formParts;
  cURLpp::options::Url url(string("https://graph.facebook.com/"+who+"/feed"));
  string token = authToken();
  string message = prompt(string("Status: "));

  formParts.push_back(new cURLpp::FormParts::Content("access_token", token));
  formParts.push_back(new cURLpp::FormParts::Content("message", message));
  
  request.setOpt(url);
  request.setOpt(new cURLpp::Options::HttpPost(formParts));
  os << request;

  Json::Value root;
  Json::Reader reader;
  bool parsingSuccessful = reader.parse(os.str(), root);
  if (!parsingSuccessful)
    {
      cerr << "Failed to parse response." << endl;
      cerr << os.str() << endl;
    }

  showErrorMessage(root);
}

/*
 * Show News Feed:
 * Shows the user's news feed. Filters out all posts that are not only
 * text so that posts that don't make sense will be filtered out. If
 * the user passes a number in the arguments that many stories will be
 * shown. This can be used to be sure the most recent stories don't
 * get pushed out of view.
 */
void ArgParse::ShowNewsFeed()
{
  // Get the JSON from Facebook
  ostringstream oss;
  cURLpp::Easy request;
  cURLpp::options::Url url(string("https://graph.facebook.com/me/home")
                         + string("?access_token=") + authToken());
  request.setOpt(url);

  oss << request;
  
  // Parse the JSON
  Json::Value root;
  Json::Reader reader;

  bool parsingSuccessful = reader.parse(oss.str(), root);
  if (!parsingSuccessful)
    {
      cerr << "Failed to parse the document." << endl;
      return;
    }

  if (showErrorMessage(root))
    {
      return;
    }

  const Json::Value posts = root["data"];
  unsigned int how_many;
  if (count > 1)
    {
      how_many = atoi(arguments[1].c_str());
      if (how_many <= 0 || how_many > posts.size())
	{
	  how_many = posts.size();
	}
    }
  else
    {
      how_many = posts.size();
    }

  stack<Json::Value> stories;

  unsigned int how_many_have_been_shown = 0;
  for (unsigned int i = 0; i < posts.size(); i ++)
    {
      string content_type = posts[i]["type"].asString();
      if (strcmp(content_type.c_str(), "status") == 0)
	{
	  stories.push(posts[i]);

	  how_many_have_been_shown ++;
	  if (how_many_have_been_shown >= how_many)
	    break;
	}
    }
  
  while (!stories.empty())
    {
      Json::Value story = stories.top();
      formatNewsStory(story, cout);
      stories.pop();
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
  cout << "       --logout          Logs the user out of Facebook"      << endl;
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

/*
 * Get Friend's ID:
 * Queries Facebook for the active user's friend list, then search
 * that list for all friends whose names contain the name
 * parameter, agregating the ID's of all matches. If more than one
 * match is found an error message is shown and NULL is returned.
 */
string ArgParse::getFriendID(string name)
{
  ostringstream os;
  cURLpp::Easy request;
  cURLpp::options::Url url(string("https://graph.facebook.com/me/friends") + string("?access_token=") + authToken());
  request.setOpt(url);

  os << request;

  Json::Value root;
  Json::Reader reader;

  bool parsingSuccessful = reader.parse(os.str(), root);
  if (!parsingSuccessful)
    {
      cerr << "Failed to parse the document." << endl;
      return "\0";
    }

  if (showErrorMessage(root))
    {
      return "\0";
    }

  vector<string> ids (0);

  const Json::Value data = root["data"];
  for (unsigned int i = 0; i < data.size(); i ++)
    {
      if (data[i]["name"].asString().find(name.c_str()) <= (unsigned int) -1)
	{
	  ids.push_back(data[i]["id"].asString());
	}
    }

  if (ids.size() > 1)
    {
      cerr << "You have more than one friend with that name." << endl;
      return "\0";
    }
  else if (ids.size() == 0)
    {
      cerr << "You don't have any friends with that name." << endl;
      return "\0";
    }
  else
    {
      return ids[0];
    }
}

/*
 * Format News Story:
 * Formats a news story by printing a nice border, the poster, and the
 * message. The message is formatted for width.
 */
void ArgParse::formatNewsStory(Json::Value story, ostream & os)
{
  string message = story["message"].asString();
  string posted_by = story["from"]["name"].asString();
  string posted_to = "\0";
  if (story["to"].size() > 0)
    {
      posted_to = story["to"]["data"]["name"].asString();
    }

  if (strcmp(message.c_str(), "") != 0)
    {
      vector<string> lines;
      string remainder = message;
      
      vector<string> new_lines;
      int nl_index = remainder.find("\n");
      while (nl_index != -1 && nl_index != (int) strlen(remainder.c_str()))
	{
	  string line = remainder.substr(0, nl_index);
	  remainder = remainder.substr(nl_index + 1);
	  new_lines.push_back(line);
	  nl_index = remainder.find("\n");
	}
      new_lines.push_back(remainder);
      
      for (unsigned int j = 0; j < new_lines.size(); j ++)
	{
	  string new_line = new_lines[j];
	  while (strlen(new_line.c_str()) > LINE_WIDTH)
	    {
	      int space_index = new_line.substr(0, LINE_WIDTH).rfind(" ");
	      string line = new_line.substr(0, space_index);
	      new_line = new_line.substr(space_index + 1);
	      lines.push_back(line);
	    }
	  lines.push_back(new_line);
	}

      os << setiosflags(std::ios::left);
      os << " " << setfill('-') << setw(LINE_WIDTH+5) << "/" << "\\" << endl;
      if (strcmp(posted_to.c_str(), "\0") == 0)
	{
	  os << " " << setfill(' ') << setw(LINE_WIDTH+5) << "| " + posted_by << "|" << endl;
	}
      else
	{
	  os << " " << setfill(' ') << setw(LINE_WIDTH+5) << "| " + posted_by + " > "
	    + posted_to << "|" << endl;
	}
      os << " " << setfill('-') << setw(LINE_WIDTH+5) << "|" << "|" << endl;
      for (unsigned int j = 0; j < lines.size(); j ++)
	{
	  os << " " << setfill(' ') << setw(LINE_WIDTH+5) << "|  " + lines[j] << "|" << endl;
	}
      os << " " << setfill('-') << setw(LINE_WIDTH+5) << "\\" << "/" << endl;
      os << endl;
    }
}


/*
 * Show Error Message:
 * If the Json document contains an error message display it and
 * return true. Return false otherwise.
 */
bool ArgParse::showErrorMessage(const Json::Value & root)
{
  string error_message = root["error"]["message"].asString();
  if (strcmp(error_message.c_str(), "\0") != 0)
    {
      cerr << error_message << endl;
      return true;
    }

  return false;
}
