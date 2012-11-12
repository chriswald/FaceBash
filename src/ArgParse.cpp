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

   else if (argHas("-k") || argHas("--like"))
      Like();
   
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
   if (argIndex(arg) == -1)
      return false;
   else
      return true;
}

/*
 * Get Argument Index:
 * Return the index of the requested argument, -1 if not found.
 */
int ArgParse::argIndex(string arg)
{
   for (int i = 0; i < count; i ++)
      if (arguments[i] == arg)
	 return i;
   return -1;
}

/*
 * Comment on Status:
 * Comment on a news story. User can specify the index of a comment as
 * well as the user whose wall the story resides on. The index
 * defaults to 1, or the most recent story. The wall defaults to the
 * user's personal news feed.
 */
void ArgParse::Comment()
{
   // Get a new journal, but don't populate it.
   Journal journal(false);

   // Set some default values.
   int index = 1;
   string friend_ID = "me";
   bool has_message_value = false;
   string message;

   // See if there are additional arguments.
   if (count > 1)
   {
      // Try and retrieve a friend's name to comment on a post on
      // their wall instead of the user's news feed. Defaults to the
      // user's news feed.
      int who_index = 0;
      if (argHas("--who"))
      {
	 who_index = argIndex("--who");
	 if (who_index < count - 1 && arguments[who_index + 1] != "me")
	 {
	    friend_ID = getFriendID(arguments[who_index + 1]);
	    if (friend_ID == "\0")
	       return;
	 }
      }

      // Try and retrieve an index number correlating to which post to
      // comment on. Defaults to the most recent.
      int num_index = 0;
      if (argHas("--num"))
      {
	 num_index = argIndex("--num");
	 if (num_index < count - 1)
	 {
	    index = atoi(arguments[num_index + 1].c_str());
	 }
      }

      int val_index = 0;
      if (argHas("--val"))
      {
	 val_index = argIndex("--val");
	 if (val_index < count - 1)
	 {
	    message = arguments[val_index + 1];
	    has_message_value = true;
	 }
      }
   }

   // Populate the journal using either the found ID or the default
   // ("me").
   journal.getNewsStories(friend_ID);
   
   // Now that I know how many stories are in the journal, bounds
   // check the index number to make sure that the story exists.
   if (index > journal.length())
   {
      cerr << "Index out of bounds." << endl;
      cerr << "Maximum value: " << journal.length() << endl;
      return;
   }
   else if (index < 1)
   {
      cerr << "Index out of bounds." << endl;
      cerr << "Minimum value: 1" << endl;
      return;
   }

   // Display the requested news story and prompt the user to enter a
   // comment.
   NewsStory story = journal[index - 1];
   if (!has_message_value)
   {
      cout << story;
      string message = Utils::prompt("Comment: ");
   }

   // Send that comment to Facebook.
   story.CommentOnStory(message);
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
   // Read login information from the user.
   LoginField login = LoginField();
   login.readUser("Email: ");
   login.readPass();
   
   int status;
   pid_t pid = fork();
   
   // Fork failed for some reason
   if (pid < 0)
   {
      perror("Fork error");
   }
   // Child process
   else if (pid == 0)
   {
      // Execute the login script by passing a username and password.
      execl("/usr/bin/python2.7",
	    "/usr/bin/python2.7",
	    "scripts/login.py",
	    login.user().c_str(),
	    login.pass().c_str(),
	    (char *) 0);
   }
   // Parent process
   else
   {
      // Wait until the child process has finished execution.
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
      
      // If the contents of member27 are longer than the maximum error
      // code length then I must have an auth token.
      if (content.length() > ERROR_CODE_LENGTH)
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
	 if      (content == "001")
	    cerr << "Invalid Email or Password." << endl;
	 
	 else if (content == "002")
	    cerr << "User denied permission to F.aceBash." << endl;
	 
	 else if (content == "003")
	    cerr << "Not connected to the Internet." << endl;
	 
	 else // Something went wrong (maybe on Facebook's end)
	    cerr << "An unknown error occured." << endl;
	 
      }
   }
   // The file could not be opened. Something went wrong on this end
   else
   {
      cerr << "Unable to login. Make sure you have proper "
	   << "local permissions." << endl;
   }
   
   // Make sure to close the file handle when I'm done.
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

void ArgParse::Like()
{
   // Get a new journal, but don't populate it.
   Journal journal(false);

   // Set some default values.
   int index = 1;
   int sub_index = 0;
   string friend_ID = "me";
   bool force_yes = false;

   // See if there are additional arguments.
   if (count > 1)
   {
      // Try and retrieve a friend's name to like a post on their wall
      // instead of the user's news feed. Defaults to the user's news
      // feed.
      int who_index = 0;
      if (argHas("--who"))
      {
	 who_index = argIndex("--who");
	 if (who_index < count - 1 && arguments[who_index + 1] != "me")
	 {
	    friend_ID = getFriendID(arguments[who_index + 1]);
	    if (friend_ID == "\0")
	       return;
	 }
      }

      // Try and retrieve an index number correlating to which post to
      // like. Defaults to the most recent.
      int num_index = 0;
      if (argHas("--num"))
      {
	 num_index = argIndex("--num");
	 if (num_index < count - 1)
	 {
	    string tmp = arguments[num_index + 1];
	    size_t period_index = tmp.find(".");
	    if (period_index == string::npos)
	    {
	       index = atoi(tmp.substr(0, period_index).c_str());
	    }
	    else
	    {
	       if (period_index == 0)
	       {
		  string s = tmp.substr(1);
		  sub_index = atoi(s.c_str());
	       }
	       else
	       {
		  index = atoi(tmp.substr(0, period_index).c_str());
		  sub_index = atoi(tmp.substr(period_index+1).c_str());
	       }
	    }
	 }
      }

      if (argHas("--force-yes"))
	 force_yes = true;
   }

   // Populate the journal using either the found ID or the default
   // ("me").
   journal.getNewsStories(friend_ID);
   
   // Now that I know how many stories are in the journal, bounds
   // check the index number to make sure that the story exists.
   if (index > journal.length())
   {
      cerr << "Index out of bounds." << endl;
      cerr << "Maximum value: " << journal.length() << endl;
      return;
   }
   else if (index < 1)
   {
      cerr << "Index out of bounds." << endl;
      cerr << "Minimum value: 1" << endl;
      return;
   }

   NewsStory story = journal[index - 1];
   int max_comment_index = story.numComments();

   if (sub_index > max_comment_index)
   {
      cerr << "Index out of bounds." << endl;
      cerr << "Number of comments on this post: " << max_comment_index << endl;
      return;
   }
   else if (sub_index < 0)
   {
      cerr << "Index out of bounds." << endl;
      cerr << "Minimum value: 1 (or no value)" << endl;
      return;
   }
   
   if (!force_yes)
   {
      cout << story;

      string reply = Utils::prompt("Like? (y/n): ");
      if (reply == "y" ||
	  reply == "Y" ||
	  reply == "yes" ||
	  reply == "Yes" ||
	  reply == "YES")
      {
	 if (sub_index == 0)
	    story.LikeStory();
	 else
	    story.LikeComment(sub_index);
      }
   }
   else
   {
      if (sub_index == 0)
	 story.LikeStory();
      else
	 story.LikeComment(sub_index);
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
   // Try to find a friend whose name matches what is being searched
   // for.
   string friend_ID = "me";
   string message;
   bool has_message_value = false;

   if (count > 1)
   {
      int who_index = 0;
      if (argHas("--who"))
      {
	 who_index = argIndex("--who");
	 if (who_index < count - 1 && arguments[who_index + 1] != "me")
	 {
	    friend_ID = getFriendID(arguments[who_index + 1]);
	    if (friend_ID == "\0")
	       return;
	 }
      }

      int val_index = 0;
      if (argHas("--val"))
      {
	 val_index = argIndex("--val");
	 if (val_index < count - 1)
	 {
	    message = arguments[val_index + 1];
	    has_message_value = true;
	 }
      }
   }
   
   stringstream ss;
   
   // Prompt the user for the status they want to post.
   if (!has_message_value)
      string message = Utils::prompt(string("Status: "));
   
   // Make request to post that status to the specified feed.
   string url = string("https://graph.facebook.com/"+friend_ID+"/feed");
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
   
   // Make sure to display any errors that Facebook may have given us.
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
   Journal journal (false);

   bool show_size_message = false;
   
   // Set some default values.
   string friend_ID = "me";
   int how_many = -1;
   
   // See if there are additional arguments.
   if (count > 1)
   {
      // Try and retrieve a friend's name to comment on a post on
      // their wall instead of the user's news feed. Defaults to the
      // user's news feed.
      int who_index = 0;
      if (argHas("--who"))
      {
	 who_index = argIndex("--who");
	 if (who_index < count - 1 && arguments[who_index + 1] != "me")
	 {
	    friend_ID = getFriendID(arguments[who_index + 1]);
	    if (friend_ID == "\0")
	       return;
	 }
      }
      
      // Try and retrieve an index number correlating to which post to
      // comment on. Defaults to the most recent.
      int num_index = 0;
      if (argHas("--num"))
      {
	 num_index = argIndex("--num");
	 if (num_index < count - 1)
	 {
	    how_many = atoi(arguments[num_index + 1].c_str());
	 }
      }
   }

   // Populate the journal using either the found ID or the default
   // ("me").
   journal.getNewsStories(friend_ID);

   // If no number of stories was read signal to display all available
   // news stories.
   if (how_many == -1)
      how_many = journal.length();
   
   if (how_many == 0)
      return;

   // Now that I know how many stories are in the journal, bounds
   // check the how_many number to make sure that all the stories
   // exist.
   if (how_many > journal.length())
   {
      show_size_message = true;
      how_many = journal.length();
   }
   else if (how_many < 1)
   {
      cerr << "Index out of bounds." << endl;
      cerr << "Minimum value: 1" << endl;
      return;
   }
   
   // Display the requested number of news stories.
   for (int i = how_many-1; i >= 0; i --)
      cout << journal[i];

   if (show_size_message)
      cout << "Output truncated to " << journal.length() << endl;
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
   cout                                                                             << endl;
   cout << "Usage: bin/Facebash [options]"                                          << endl;
   cout                                                                             << endl;
   cout << "Options:"                                                               << endl;
   cout << "   -c,  --comment   Adds comment to the nth most recent post."          << endl;
   cout << "                    [default most recent]"                              << endl;
   cout << "   -h, --help       Shows this message"                                 << endl;
   cout << "   -l, --login      Logs a user into Facebook"                          << endl;
   cout << "       --logout     Logs the user out of Facebook"                      << endl;
   cout << "   -k, --like       Likes a specific news story."                       << endl;
   cout << "   -n, --show_news_feed"                                                << endl;
   cout << "                    Shows n most recent items from the current user's"  << endl;
   cout << "                    news feed. [default to all in one page.]"           << endl;
   cout << "   -s, --update_status"                                                 << endl;
   cout << "                    Updates the current user's status"                  << endl;
   cout << "   -v, --version    Displays version information"                       << endl;
   cout                                                                             << endl;
}

/*
 * Show Version Information:
 * Displays the program's current version and copyright information.
 */
void ArgParse::ShowVersion()
{
   cout << "F.aceBash v" << version                 << endl;
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
      cerr << "You have more than one friend with that name: " << name << endl;
      return "\0";
   }
   else if (ids.size() == 0)
   {
      cerr << "You don't have any friends with that name: " << name << endl;
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
