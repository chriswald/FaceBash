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

  // Add a comment to a specified post.
  if      (argHas("-c") || argHas("--comment"))
    Comment();

  // The user specifically requested the help text. This also displays
  // the version info.
  else if (argHas("-h") || argHas("--help"))
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
 * Comment on Status:
 *
 */
void ArgParse::Comment()
{
  Journal journal(true);

  int index = 1;
  if (count > 1)
    {
      index = atoi(arguments[1].c_str());
    }

  if (index > journal.length())
    {
      cout << "Index out of bounds." << endl;
      cout << "Maximum value: " << journal.length() << endl;
      return;
    }
  else if (index < 1)
    {
      cout << "Index out of bounds." << endl;
      cout << "Minimum value: 1" << endl;
      return;
    }

  NewsStory story = journal[index-1];
  cout << story;

  string message = Utils::prompt("Comment: ");

  story.Comment(message);
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

      if (strlen(content.c_str()) > ERROR_CODE_LENGTH)
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
	  if      (strcmp(content.c_str(), "001") == 0)
	      cout << "Invalid Email or Password." << endl;

	  else if (strcmp(content.c_str(), "002") == 0)
	      cout << "User denied permission to F.aceBash." << endl;

	  else if (strcmp(content.c_str(), "003") == 0)
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
  string message = Utils::prompt(string("Status: "));
  string url = string("https://graph.facebook.com/"+who+"/feed");
  cURLpp::Forms formParts;
  formParts.push_back(new cURLpp::FormParts::Content("message", message));

  bool request_success = NetUtils::makeRequest(ss, url, formParts);

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

  NetUtils::showErrorMessage(root);
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
  Journal journal(true);
  int how_many = journal.length();

  if (count > 1)
    {
      how_many = atoi(arguments[1].c_str());
    }
  if (how_many < 1)
    {
      cout << "Invalid argument: Minimum value of 1" << endl;
      return;
    }
  else if (how_many > journal.length())
    {
      cout << "Invaild argument: Maximum value of " << journal.length() << endl;
      return;
    }
  
  cout << "Showing your news feed:" << endl;
  
  if (how_many < journal.length())
    {
      for (int i = how_many-1; i >= 0; i --)
	cout << journal[i];
    }
  else
    {
      cout << journal;
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
  bool request_success = NetUtils::makeRequest(ss, url);

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

  int error_code = NetUtils::showErrorMessage(root);
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
 * ReLogin:
 * Uuuhhhhhh, yeah this might do something at some point.
 */
bool ArgParse::relogin()
{
  return false;
}
