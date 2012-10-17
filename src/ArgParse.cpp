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
  // The user didn't enter any arguments. Just display the help text
  // (also displays the version info).
  if (count == 0)
    {
      ShowHelpText();
      return;
    }

  // The user specifically requested the help text. This also displays
  // the version info.
  if      (argHas("-h") || argHas("--help"))
    ShowHelpText();

  // The user wants to login to F.aceBash / Facebook.
  else if (argHas("-l") || argHas("--login"))
    Login();

  // The user want to log out of F.aceBash / Facebook.
  else if (                argHas("--logout"))
    Logout();

  // Show the user's news feed.
  else if (argHas("-n") || argHas("--show_news_feed"))
    ShowNewsFeed();

  // Allow the user to post a status on either their own wall or the
  // wall of a friend.
  else if (argHas("-s") || argHas("--update_status"))
    UpdateStatus();

  // Show the version information (without help text).
  else if (argHas("-v") || argHas("--version"))
    ShowVersion();

  // The user entered some argument that isn't supported or
  // recognized. Show the help text (with version information).
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
  
  if (pid < 0)        // Fork failed for some reason
    {
      perror("Fork error");
    }
  else if (pid == 0)  // Child process
    {
      execl("/usr/bin/python2.7", "/usr/bin/python2.7", "scripts/login.py", login.user().c_str(), login.pass().c_str(), (char *) 0);
    }
  else                // Parent process
    {
      if ((pid = wait(&status)) == -1)
	perror("wait error");
    }
  
  // Checks to see whether the login was successful by open the
  // member27 file and checking its contents. If something that looks
  // like an authentification token is found that's good. Otherwise
  // display a message relating to the error that was found.
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
	  /*
	   * Error Conditions:
	   *  1) The user entered an invalid username (email address)
	   *     or password when logging in.
	   *  2) The user did not grant permission to F.aceBash to
	   *     access their account (eg selected no instead of yes
	   *     when asked to grant permission).
	   *  3) The user is not connected to the internet.
	   */
	  if      (strcmp(content.c_str(), "1") == 0)
	      cout << "Invalid Email or Password." << endl;

	  else if (strcmp(content.c_str(), "2") == 0)
	      cout << "User denied permission to F.aceBash." << endl;

	  else if (strcmp(content.c_str(), "3") == 0)
	      cout << "Not connected to the Internet." << endl;

	  else // Something went wrong (maybe on Facebook's end)
	      cout << "An unknown error occured." << endl;

	}
    }
  else // The file could not be opened. Something went wrong on this end
    {
      cerr << "Unable to login. Make sure you have proper "
	   << "local permissions." << endl;
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
  // Try to find a friend whose name matches what is being searched
  // for.
  string who;
  if (count > 1)
    {
      who = getFriendID(arguments[1]);
      if (strcmp(who.c_str(), "\0") == 0)
	{
	  // Found more or less than one friend with that name?
	  // Just return.
	  return;
	}
    }
  else
    {
      // No name passed as a parameter?
      // Default to the current user.
      who = "me";
    }

  stringstream ss;
  string message = prompt(string("Status: "));
  string url = string("https://graph.facebook.com/"+who+"/feed");
  cURLpp::Forms formParts;
  formParts.push_back(new cURLpp::FormParts::Content("message", message));

  bool request_success = makeRequest(ss, url, formParts);

  // If the request wasn't successfully made just return. Some
  // function previous to this should have displayed some error
  // message.
  if (!request_success)
    {
      return;
    }

  Json::Value root;
  Json::Reader reader;
  bool parsingSuccessful = reader.parse(ss.str(), root);
  if (!parsingSuccessful)
    {
      cerr << "Failed to parse response." << endl;
      cerr << ss.str() << endl;
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
  stringstream ss;
  string url = string("https://graph.facebook.com/me/home");
  bool request_success = makeRequest(ss, url);

  // If the request wasn't successfully made just return. Some
  // function previous to this should have displayed some error
  // message.
  if (!request_success)
    {
      return;
    }

  // Parse the JSON
  Json::Value root;
  Json::Reader reader;

  bool parsingSuccessful = reader.parse(ss.str(), root);
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
      // Converts the first argument to an integer
      how_many = atoi(arguments[1].c_str());

      // If the requested number of posts is out of range then just
      // set it to the maximum available.
      if (how_many <= 0 || how_many > posts.size())
	{
	  how_many = posts.size();
	}
    }
  else
    {
      // If no number of posts is specified by the user just set it to
      // the maximum available.
      how_many = posts.size();
    }

  stack<Json::Value> stories;

  // Keep track of how many posts have actually been shown, not just
  // how many to try to show.
  unsigned int how_many_have_been_shown = 0;

  for (unsigned int i = 0; i < posts.size(); i ++)
    {
      string content_type = posts[i]["type"].asString();

      // Make sure the post is a status and isn't empty
      if (strcmp(content_type.c_str(), "status") == 0 &&
	  strlen(posts[i]["message"].asString().c_str()) > 0)
	{
	  // Push the story onto a stack to display later.
	  stories.push(posts[i]);
	  // Then increment how many stories "have been shown". The
	  // stories will actually be shown later.
	  how_many_have_been_shown ++;

	  // If the number of accepted stories reaches the number of
	  // stories available break out of the loop.
	  if (how_many_have_been_shown >= how_many)
	    break;
	}
    }
  
  // While there are stories still on the stack...
  while (!stories.empty())
    {
      // Get the top story and pass it for formatting along with an
      // index number that can be used for referencing it later. Then
      // remove it from the stack.
      Json::Value story = stories.top();
      formatNewsStory(cout, story, how_many_have_been_shown--);
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
 * returned. On failure \0 is returned.
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
      return "\0";
    }
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
  // Query Facebook for a list of all the user's friends.
  stringstream ss;
  string url = string("https://graph.facebook.com/me/friends");
  bool request_success = makeRequest(ss, url);

  if (!request_success)
    {
      return "\0";
    }

  // Parse said list.
  Json::Value root;
  Json::Reader reader;

  bool parsingSuccessful = reader.parse(ss.str(), root);

  // Make sure no errors occurred.
  if (!parsingSuccessful)
    {
      cerr << "Failed to parse the document." << endl;
      return "\0";
    }

  int error_code = showErrorMessage(root);
  if (error_code)
    {
      bool relogin_success = relogin();
      if (!relogin_success)
	{
	  return "\0";
	}
    }

  vector<string> ids (0);

  // Search throught the returned friend list for all names matching
  // the name parameter.
  const Json::Value data = root["data"];
  for (unsigned int i = 0; i < data.size(); i ++)
    {
      if (data[i]["name"].asString().find(name.c_str()) <= (unsigned int) -1)
	{
	  ids.push_back(data[i]["id"].asString());
	}
    }

  // If the number of matches is not exactly one display an error
  // message and return \0. Otherwise return the ID of the one match.
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
void ArgParse::formatNewsStory(ostream & os, Json::Value story, int index)
{
  string message = story["message"].asString();
  string posted_by = story["from"]["name"].asString();

  // This conditional is probably unnecessarry since the same
  // condition is being checked in ShowNewsFeed, but its also not
  // hurting anything so I'll keep it.
  if (strlen(message.c_str()) > 0)
    {
      vector<string> lines;
      string remainder = message;
      
      vector<string> new_lines;
      // Break the message body at newlines.
      int nl_index = remainder.find("\n");
      while (nl_index != -1 && nl_index != (int) strlen(remainder.c_str()))
	{
	  string line = remainder.substr(0, nl_index);
	  remainder = remainder.substr(nl_index + 1);
	  new_lines.push_back(line);
	  nl_index = remainder.find("\n");
	}
      new_lines.push_back(remainder);
      
      // Run through each of those lines and make sure it does not
      // exceed LINE_WIDTH. Split it into sub lines until it does.
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

      // Write each individual portion of the story.
      writeSeperatorLine(os);
      writeNameLine(os, posted_by, index);
      writeSeperatorLine(os);
      writeMessageLines(os, lines);
      writeSeperatorLine(os);
      os << endl;
    }
}

/*
 * Write Message Line:
 * Writes each line from lines to os with leading characters and
 * trailing characters with enough spaces to evenly align everything.
 */
void ArgParse::writeMessageLines(ostream & os, const vector<string> lines)
{
  for (unsigned int i = 0; i < lines.size(); i ++)
    {
      stringstream ss;
      ss << "|  " << lines[i];
      int len = strlen(ss.str().c_str());
      for (int j = len; j < LINE_WIDTH+5; j ++)
	ss << " ";
      ss << "|" << endl;
      os << ss.str();
    }
}

/*
 * Write Name Line:
 * Writes the name (left aligned) and index (right aligned) to os. The
 * two values are written with a fixed width of LINE_WIDTH. Leading
 * and trailing characters are also printed.
 */
void ArgParse::writeNameLine(ostream & os, const string & name, const int index)
{
  stringstream ss;
  stringstream sindex;
  sindex << index << " ";
  ss << "| " << name;
  int len = strlen(ss.str().c_str());
  for (unsigned int i = len; i < LINE_WIDTH+5 - strlen(sindex.str().c_str()); i ++)
    ss << " ";
  ss << sindex.str() << "|" << endl;
  os << ss.str();
}
  
/*
 * Write Seperator Line:
 * Writes a line of width LINE_WIDTH used as a filler / seperator
 * between elements.
 */
void ArgParse::writeSeperatorLine(ostream & os)
{
  stringstream ss;
  ss << "|";
  for (int i = 1; i < LINE_WIDTH+5; i ++)
    ss << "-";
  ss << "|" << endl;
  os << ss.str();
}    

/*
 * Show Error Message:
 * If the Json document contains an error message display it and
 * return true. Return false otherwise.
 */
int ArgParse::showErrorMessage(const Json::Value & root)
{
  string error_message = root["error"]["message"].asString();
  if (strcmp(error_message.c_str(), "\0") != 0)
    {
      cerr << error_message << endl;
      return root["error"]["code"].asInt();
    }

  return 0;
}

/*
 * ReLogin:
 * Uuuhhhhhh, yeah this might do something at some point.
 */
bool ArgParse::relogin()
{
  return false;
}

/*
 * Make Request:
 * Makes a request to the given url and writes the return to
 * ss. Returns true if the request was successful, false otherwise.
 */
bool ArgParse::makeRequest(stringstream & ss, const string & url)
{
  string token = authToken();

  if (strcmp(token.c_str(), "\0") == 0)
    {
      cout << "Please Log in." << endl;
      return false;
    }

  cURLpp::Easy request;
  cURLpp::options::Url URL(url + string("?access_token=") + token);
  request.setOpt(URL);

  try
    {
      ss << request;
    }
  catch (curlpp::LibcurlRuntimeError & e)
    {
      cout << e.what() << endl;
      return false;
    }

  return true;
}

/*
 * Make Request:
 * Makes a request to the given url with the given form data and
 * writes the return to ss. Returns true if the request was
 * successful, false otherwise.
 */
bool ArgParse::makeRequest(stringstream & ss, const string & url, const cURLpp::Forms & formParts)
{
  string token = authToken();

  if (strcmp(token.c_str(), "\0") == 0)
    {
      cout << "Please Log in." << endl;
      return false;
    }

  cURLpp::Easy request;
  cURLpp::options::Url URL(url + string("?access_token=") + token);
  request.setOpt(URL);
  request.setOpt(new cURLpp::Options::HttpPost(formParts));

  try
    {
      ss << request;
    }
  catch (curlpp::LibcurlRuntimeError & e)
    {
      cout << e.what() << endl;
      return false;
    }

  return true;
}
