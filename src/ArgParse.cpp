#include "ArgParse.hpp"

/*
 * Constructor:
 * Sets the count to the number of arguments and copies all arguments
 * to to an array of strings. Does not copy the first argument, the
 * program name.
 */
ArgParse::ArgParse(int argc, char **argv)
{
   vector<string> cmds;

   count = argc-1;
   arguments = new string[count];
   for (int i = 0; i < count; i ++)
   {
      string tmp = string(argv[i+1]);
      arguments[i] = tmp;
      if (tmp[0] == '-')
      {
	 if (tmp[1] == '-')
	 {
	    addCommand(tmp.substr(2), 1);
	 }
	 else
	 {
	    int times_to_add = 0;
	    string command_to_add = "";
	    for (unsigned int j = 1; j < tmp.length(); j ++)
	    {
	       if (tmp[j] >= '0' && tmp[j] <= '9')
	       {
		  times_to_add = times_to_add * 10 + (tmp[j] - '0');
	       }
	       else
	       {
		  if (command_to_add.length() > 0)
		  {
		     addCommand(command_to_add, times_to_add);
		  }
		  command_to_add = tmp[j];
	       }
	    }
	    addCommand(command_to_add, times_to_add);
	 }
      }
   }
}

/*
 * Destructor:
 * Returns arguments array to the heap.
 */
ArgParse::~ArgParse()
{
   delete[] arguments;
}

void ArgParse::addCommand(const string & command, int times)
{
   int times_added = 0;
   do
   {
      commands.push_back(command);
      times_added ++;
   } while (times_added < times);
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

   for (unsigned int i = 0; i < commands.size(); i ++)
   {
      string cmd = commands[i];
      
      if      (cmd == "a" || cmd == "notifications")
	 ShowNotifications();

      else if (cmd == "c" || cmd == "comment")
	 Comment();
      
      else if (cmd == "h" || cmd == "help")
	 ShowHelpText();

      else if (cmd == "l" || cmd == "login")
	 Login();

      else if (              cmd == "logout")
	 Logout();

      else if (cmd == "k" || cmd == "like")
	 Like();

      else if (cmd == "n" || cmd == "show_news_feed")
	 ShowNewsFeed();

      else if (cmd == "s" || cmd == "update_status")
	 UpdateStatus();

      else if (cmd == "u" || cmd == "upload")
	 Upload();

      else if (cmd == "v" || cmd == "version")
	 ShowVersion();
   }
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
 * Show Notifications:
 * 
 */
void ArgParse::ShowNotifications()
{
   bool plain_format = false;
   
   if (count > 1)
   {
      if (argHas("--plain"))
	 plain_format = true;
   }

   NotifLog log = NotifLog(plain_format);   
   bool request_success = true;
   request_success = log.getNotifications();

   if (!request_success)
      return;

   cout << log;
}

/*
 * Comment on Status:
 * Comment on a news story. User can specify the index of a comment as
 * well as the user whose wall the story resides on. The index
 * defaults to 1, or the most recent story. The wall defaults to the
 * user's personal news feed. Responds to the following arguments:
 * who [name] - The name of the friend whose wall the item is on. Can
 *   be a friend's name, "me", or blank. If "me" is passed user's own
 *   wall is used. If left blank the user's news feed is used.
 * num [number] - The index number of the post to comment on. This
 *   number is shown in the head line of each post. It is used in
 *   combination with the "who" parameter to specify posts on
 *   different walls.
 * val [message] - The text of the comment. If this is provided the
 *   user is not prompted for input and the chosen post is not
 *   displayed.
 */
void ArgParse::Comment()
{
   // Get a new journal, but don't populate it.
   Journal journal(false);

   // Set some default values.
   int index = 1;
   string friend_ID = "\0";
   bool has_message_value = false;
   bool plain_format = false;
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
	 if (who_index < count - 1)
	 {
	    if (!NetUtils::getFriendID(arguments[who_index + 1], friend_ID))
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

      if (argHas("--plain"))
      {
	 plain_format = true;
      }
   }

   // Populate the journal using either the found ID or the default
   // ("me").
   journal.setFormatStyle(plain_format);
   journal.getNewsStories(friend_ID);
   
   // Now that I know how many stories are in the journal, bounds
   // check the index number to make sure that the story exists.
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

   // Display the requested news story and prompt the user to enter a
   // comment.
   NewsStory story = journal[index - 1];
   if (!has_message_value)
   {
      cout << story;
      message = Utils::prompt("Comment: ");
   }

   // Send that comment to Facebook.
   story.CommentOnStory(message);
}

/*
 * Like:
 * Tells Facebook to add your stamp of approval to a status or
 * comment. Responds to the following arguments:
 * who [name] - The name of the friend whose wall the item is on. Can
 *   be a friend's name, "me", or blank. If "me" is passed user's own
 *   wall is used. If left blank the user's news feed is used.
 * num [number] - The index value of the item to Like. This is
 *   displayed next to the item. This number has a specific format of
 *   x(.y) where the first number, x, is the index of the post to Like
 *   and y is the index of the comment to Like if applicable. If no x
 *   is provided, but a .y is, the system assumes a value of x=1 and
 *   tries to Like comment 1.y. If no .y is provided the system tries
 *   to Like post x. If x. is provided the system ignores the '.' and
 *   tries to like post x. If no value is provided the system assumes
 *   a value of 1, and so tries to Like post 1 on the specified
 *   stream.
 * force-yes - If present the system skips the confirmation step and
 *   automatically Likes the specified item.
 */
void ArgParse::Like()
{
   // Get a new journal, but don't populate it.
   Journal journal(false);

   // Set some default values.
   int index = 1;
   int sub_index = 0;
   string friend_ID = "\0";
   bool force_yes = false;
   bool plain_format = false;

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
	 if (who_index < count - 1)
	 {
	    if (!NetUtils::getFriendID(arguments[who_index + 1], friend_ID))
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

      if (argHas("--force-yes") || argHas("--force_yes"))
	 force_yes = true;

      if (argHas("--plain"))
      {
	 plain_format = true;
      }
   }

   // Populate the journal using either the found ID or the default
   // ("me").
   journal.setFormatStyle(plain_format);
   journal.getNewsStories(friend_ID);
   
   // Now that I know how many stories are in the journal, bounds
   // check the index number to make sure that the story exists.
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

   NewsStory story = journal[index - 1];
   int max_comment_index = story.numComments();

   if (sub_index > max_comment_index)
   {
      cout << "Index out of bounds." << endl;
      cout << "Number of comments on this post: " << max_comment_index << endl;
      return;
   }
   else if (sub_index < 0)
   {
      cout << "Index out of bounds." << endl;
      cout << "Minimum value: 1 (or no value)" << endl;
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
 * Log In:
 * Handles all responsibilities associated with retrieving a valid
 * authentification token. The user is prompted for their email and
 * password, then the process is forked in order to call a python
 * script which handles the logging in. The parent process waits for
 * the script to exit, then runs a check to see if the
 * authentification token exists, informing the user whether the login
 * completed successfully or not based on the results of that
 * check. Responds to the following arguments:
 * user [username] - The user name to login with. This should be an
 *   email address.
 * pass [password] - The user's password. It is recommended not to use
 *   this flag as the password will be visible in history as well as
 *   'w' and 'who' (among other) commands, but can be useful for
 *   scripting. Don't say you weren't warned.
 */
void ArgParse::Login()
{
   string user = "\0";
   string pass = "\0";

   if (count > 1)
   {
      int user_index = 0;
      if (argHas("--user"))
      {
	 user_index = argIndex("--user");
	 if (user_index < count - 1)
	 {
	    user = string(arguments[user_index+1]);
	 }
      }

      int pass_index = 0;
      if (argHas("--pass"))
      {
	 pass_index = argIndex("--pass");
	 if (pass_index < count - 1)
	 {
	    pass = string(arguments[pass_index+1]);
	 }
      }
   }

   // Read login information from the user.
   LoginField login = LoginField();

   if (user == "\0")
   {
      login.readUser("Email: ");
      user = login.user();
   }

   if (pass == "\0")
   {
      login.readPass();
      pass = login.pass();
   }
   
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
      string script = PKGPYEXECDIR;
      script += "/login.pyc";
      execl("/usr/bin/python2.7",
	    "/usr/bin/python2.7",
	    script.c_str(),
	    user.c_str(),
	    pass.c_str(),
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
   string filename = Utils::authFile();
   member27.open(filename.c_str());
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
	  *  4) The user sent a SIGINT to the process during login.
	  */
	 if      (content == "001")
	    cout << "Invalid Email or Password." << endl;
	 
	 else if (content == "002")
	    cout << "User denied permission to F.aceBash." << endl;
	 
	 else if (content == "003")
	    cout << "Not connected to the Internet." << endl;

	 else if (content == "004")
	    cout << "Login process interrupted." << endl;

	 else if (content == "005")
	    cout << "Bad arguments. Reinstalling may help." << endl;
	 
	 else // Something went wrong (maybe on Facebook's end)
	    cout << "An unknown error occured." << endl;
	 
      }
   }
   // The file could not be opened. Something went wrong on this end
   else
   {
      cout << "Unable to login. Make sure you have proper "
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
   remove(Utils::authFile().c_str());
}

/*
 * Show News Feed:
 * Shows the user's news feed. Filters out all posts that are not only
 * text so that posts that don't make sense will be filtered out. If
 * the user passes a number in the arguments that many stories will be
 * shown. This can be used to be sure the most recent stories don't
 * get pushed out of view. Responds to the following arguments:
 * who [name] - The name of the friend whose wall the item is on. Can
 *   be a friend's name, "me", or blank. If "me" is passed user's own
 *   wall is used. If left blank the user's news feed is used.
 * num [number] - A range of numbers representing the posts to
 *   show. See documentation on parseRange for information on
 *   formatting a range.
 */
void ArgParse::ShowNewsFeed()
{
   Journal journal (false);

   // Set some default values.
   string friend_ID = "\0";
   bool size_spec = false;
   set<int> indices;
   bool plain_format = false;

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
	 if (who_index < count - 1)
	 {
	    if (!NetUtils::getFriendID(arguments[who_index + 1], friend_ID))
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
	    size_spec = true;
	    bool b = parseRange(arguments[num_index + 1], indices);
	    if (!b)
	       return;
	 }
      }

      if (argHas("--plain"))
      {
	 plain_format = true;
      }
   }

   // Populate the journal using either the found ID or the default
   // ("me").
   journal.setFormatStyle(plain_format);
   journal.getNewsStories(friend_ID);

   if (size_spec)
   {
      set<int>::reverse_iterator itr;
      for (itr = indices.rbegin(); itr != indices.rend(); itr ++)
      {
	 if (*itr < journal.length())
	 {
	    cout << journal[(*itr)];
	 }
      }
   }
   else
   {
      cout << journal;
   }
}

/*
 * Update Status:
 * Handles all tasks associated with allowing a user to update his or
 * her status. Prompts the user for a status message, then polls for
 * the authentification token, then submits a POST to the Facebook
 * Graph API. Responds to the following arguments:
 * who [name] - The name of the friend whose wall the item is on. Can
 *   be a friend's name, "me", or blank. If "me" is passed user's own
 *   wall is used. If left blank the user's news feed is used.
 * val [message] - The text of the status update. If this is provided
 *   the user is not promped for input.
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
	 if (who_index < count - 1)
	 {
	    if (!NetUtils::getFriendID(arguments[who_index + 1], friend_ID))
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
      message = Utils::prompt(string("Status: "));
   
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
      cout << "Failed to parse response." << endl;
      cout << ss.str() << endl;
   }
   
   NetUtils::showErrorMessage(root);
}

/*
 * Upload:
 * Batch uploads photos to an album. User can specify photos to be
 * uploaded, the name of the album to upload to, and the description
 * of that album. The default album is "F.aceBash", with no
 * description. The user can also choose to only display a list of
 * existing albums. If the album exists already any specified message
 * will be ignored. Images can be uploaded individually or batch.
 * Responds to the following arguments:
 * album [name] - The name of the album to upload photos to. If none
 *   provided "F.aceBash" is used by default. If the specified album
 *   does not exist it is created. Videos do not get uploaded to
 *   albums so if only videos are being uploaded this argument is not
 *   needed and will just be ignored if provided.
 * val [message] - The description to apply to the album. If a new
 *   album is created this message is passed as the description of the
 *   new album. If the album alread exists this value is ignored and
 *   the user is notified.
 * list - If this flag is set all other arguments are ignored and the
 *   only action taken is to display the names of all existing
 *   albums.
 * img [files...] - This specifies the list of files to upload to
 *   whichever album has been specified. This list can contain any
 *   number of image files. No code is implemented client-side to
 *   check valid file types.
 * vid [files...] - This specifies the list of files to upload to
 *   Facebook as videos. This list can contain any number of video
 *   files. No code is implemented client-side to check valid file
 *   types.
 */
void ArgParse::Upload()
{
   // Set some meaningful default values.
   string album_name = "F.aceBash";
   string album_id = "";
   string message = "";
   bool album_exists = false;
   bool list_names = false;
   int begin_img_index = 0;
   int num_imgs = 0;
   int begin_vid_index = 0;
   int num_vids = 0;

   // Check for command line arguments. If none are found simply list
   // existing album names.
   if (count > 1)
   {
      int album_index = argIndex("--album");
      int msg_index   = argIndex("--val");
      int img_index   = argIndex("--img");
      int vid_index   = argIndex("--vid");

      // Retrieve the album title.
      if (argHas("--album"))
      {
	 if (album_index < count - 1)
	 {
	    album_name = arguments[album_index + 1];
	 }
      }

      // Retrieve the album description message.
      if (argHas("--val"))
      {
	 if (msg_index < count - 1)
	 {
	    message = arguments[msg_index + 1];
	 }
      }

      // Only list album names.
      if (argHas("--list"))
      {
	 list_names = true;
      }

      // Generate a list of images to be uploaded.
      if (argHas("--img"))
      {
	 if (img_index < count - 1)
	 {
	    begin_img_index = img_index + 1;
	    for (int i = begin_img_index; i <= count; i ++)
	    {
	       if (arguments[i][0] == '-' || i == count)
	       {
		  num_imgs = (i - 1) - img_index;
		  break;
	       }
	    }
	 }
      }

      // Generate a list of videos to be uploaded.
      if (argHas("--vid"))
      {
	 if (vid_index < count - 1)
	 {
	    begin_vid_index = vid_index + 1;
	    for (int i = begin_vid_index; i <= count; i ++)
	    {
	       if (arguments[i][0] == '-' || i == count)
	       {
		  num_vids = (i - 1) - vid_index;
		  break;
	       }
	    }
	 }
      }
   }
   else
   {
      list_names = true;
   }

   // Get a listing of all of the user's albums.
   map<string, string> names;
   bool success = getAlbumNames(names);
   if (!success)
      return;

   // If the --list flag was set display a list of all names and
   // return.
   map<string, string>::iterator itr;
   if (list_names)
   {
      for (itr = names.begin(); itr != names.end(); itr ++)
      {
	 cout << itr->first << endl;
      }
      return;
   }

   // If I get here then the user must be trying to upload photos. I
   // need to make sure that some photo files have been provided.
   if (num_imgs == 0 && num_vids == 0)
   {
      cout << "No files specified!" << endl;
      return;
   }

   if (num_imgs > 0)
   {
      // Iterate over the album names to see if one matches the requested
      // name. If so retrieve its ID.
      for (itr = names.begin(); itr != names.end(); itr ++)
      {
	 if (itr->first == album_name)
	 {
	    album_exists = true;
	    album_id = itr->second;
	 }
      }
      
      // If I didn't find the album requested, make it and retrieve its
      // ID. Otherwise the ID was retrieved earlier. Just let the user
      // know that any message provided won't be applied.
      if (!album_exists)
      {
	 bool success = makeAlbum(album_name, message, album_id);
	 if (!success)
	    return;
      }
      else
      {
	 if (message != "")
	    cout << "Album exists, not applying message." << endl;
      }
      
      // Let the user know which album is being uploaded to.
      cout << "Album: " << album_name << endl;

      // Parse the arguments, adding each image file to a vector.
      vector<string> images;
      for (int i = 0; i < num_imgs; i ++)
	 images.push_back(arguments[begin_img_index + i]);

      // Upload all images to Facebook.
      success = sendImagesToAlbum(album_id, images);
      if (!success)
	 cout << "Image upload was not completely successful." << endl;
   }

   if (num_vids > 0)
   {
      // Parse the arguments, add each video file to a vector.
      vector<string> videos;
      for (int i = 0; i < num_vids; i ++)
	 videos.push_back(arguments[begin_vid_index + i]);
      
      // Upload all videos to Facebook.
      success = sendVideosToFacebook(videos);
      if (!success)
	 cout << "Video upload was not completely successful." << endl;
   }
}

/*
 * Send Images to Facebook Album:
 * Takes a vector of image files and uploads them to an album with the
 * provided ID. Does no file type checking.
 */
bool ArgParse::sendImagesToAlbum(const string & ID, const vector<string> & files)
{
   // Keep track of overall operation success.
   bool success = true;

   // Iterate over all files sent.
   for (unsigned int i = 0; i < files.size(); i ++)
   {
      // Keep track of success on this single upload operation.
      bool small_success = true;

      // Get the image's full name (with path) and simple name
      // (without path).
      string file = files[i];
      string filename = file.substr(file.rfind("/") + 1);

      // Display some diagnostic info to the user.
      cout << "Uploading <" << filename << ">: ... " << std::flush;

      // Add the file to a cURLpp form and send it on its way. The
      // rest of the process is just book-keeping and error checking.
      stringstream ss;
      string url = string("https://graph.facebook.com/") + ID + string("/photos");
      cURLpp::Forms formParts;
      formParts.push_back(new cURLpp::FormParts::File(filename, file));
      bool request_success = NetUtils::makeRequest(ss, url, formParts);

      // An error message should have been shown previous to this point.
      if (!request_success)
	 small_success = false;
      
      // Make sure the process was still successful up to this point.
      if (small_success)
      {
	 // Parse the returned information.
	 Json::Value root;
	 Json::Reader reader;
	 bool parsingSuccessful = reader.parse(ss.str(), root);
	 
	 // Make sure no parsing errors occurred.
	 if (!parsingSuccessful)
	 {
	    cout << "Failed to parse the document." << endl;
	    small_success = false;
	 }
	 else
	 {
	    // Show any Facebook generated error messages.
	    int error_code = NetUtils::showErrorMessage(root);
	    if (error_code)
	       small_success = false;
	 }
      }

      // If this single operation is successful finish the message
      // that was show to the user with a helpful "Done". Otherwise
      // display "Error" and set global success to false.
      if (small_success)
      {
	 cout << "Done" << endl;
      }
      else
      {
	 cout << "Error" << endl;
	 success = false;
      }
   }
   return success;
}

/*
 * Send Videos to Facebook:
 * Takes a vector of video files and uploads them to Facebook. Does no
 * file type checking.
 */
bool ArgParse::sendVideosToFacebook(const vector<string> & files)
{
   // Keep track of overall operation success.
   bool success = true;

   // Iterate over all files sent.
   for (unsigned int i = 0; i < files.size(); i ++)
   {
      // Keep track of success on this single upload operation.
      bool small_success = true;

      // Get the image's full name (with path) and simple name
      // (without path).
      string file = files[i];
      string filename = file.substr(file.rfind("/") + 1);

      // Display some diagnostic info to the user.
      cout << "Uploading <" << filename << ">: ... " << std::flush;

      // Add the file to a cURLpp form and send it on its way. The
      // rest of the process is just book-keeping and error checking.
      stringstream ss;
      string url = "https://graph.facebook.com/me/videos";
      cURLpp::Forms formParts;
      formParts.push_back(new cURLpp::FormParts::File(filename, file));
      bool request_success = NetUtils::makeRequest(ss, url, formParts);

      // An error message should have been shown previous to this point.
      if (!request_success)
	 small_success = false;
      
      // Make sure the process was still successful up to this point.
      if (small_success)
      {
	 // Parse the returned information.
	 Json::Value root;
	 Json::Reader reader;
	 bool parsingSuccessful = reader.parse(ss.str(), root);
	 
	 // Make sure no parsing errors occurred.
	 if (!parsingSuccessful)
	 {
	    cout << "Failed to parse the document." << endl;
	    small_success = false;
	 }
	 else
	 {
	    // Show any Facebook generated error messages.
	    int error_code = NetUtils::showErrorMessage(root);
	    if (error_code)
	       small_success = false;
	 }
      }

      // If this single operation is successful finish the message
      // that was show to the user with a helpful "Done". Otherwise
      // display "Error" and set global success to false.
      if (small_success)
      {
	 cout << "Done" << endl;
      }
      else
      {
	 cout << "Error" << endl;
	 success = false;
      }
   }
   return success;
}

/*
 * Get Album Names:
 * Generates a string string map where the album name is the key and
 * the album ID is the value. Returns true if the operation was
 * successful, false otherwise.
 */
bool ArgParse::getAlbumNames(map<string, string> & names)
{
   // Make a requst for the album listing to Facebook.
   stringstream ss;
   string get_albums_url = "https://graph.facebook.com/me/albums";
   bool request_success = NetUtils::makeRequest(ss, get_albums_url);

   // An error message should have been shown previous to this point.
   if (!request_success)
      return false;

   // Parse the response.
   Json::Value root;
   Json::Reader reader;
   bool parsingSuccessful = reader.parse(ss.str(), root);
   
   // Make sure no parsing errors occurred.
   if (!parsingSuccessful)
   {
      cout << "Failed to parse the document." << endl;
      return false;
   }
   
   // Show any errors raised by Facebook.
   int error_code = NetUtils::showErrorMessage(root);
   if (error_code)
      return false;

   // Parse the (now know to be valid) JSON for album names and IDs,
   // storing each pair as a new element in the map.
   Json::Value data = root["data"];
   for (unsigned int i = 0; i < data.size(); i ++)
   {
      string name = data[i]["name"].asString();
      string id = data[i]["id"].asString();
      names[name] = id;
   }

   // Got to the end so everything must have been successful.
   return true;
}

/*
 * Makes Album:
 * Creates a new album on Facebook with the given name and
 * message. Returns true on successful completion, false otherwise.
 */
bool ArgParse::makeAlbum(const string & name, const string & message, string & ID)
{
   // Build a form with the name and description message and POST that
   // request to Facebook.
   stringstream ss;
   string url ("https://graph.facebook.com/me/albums");
   cURLpp::Forms formParts;
   formParts.push_back(new cURLpp::FormParts::Content("name", name));
   if (message.length() > 0)
      formParts.push_back(new cURLpp::FormParts::Content("message", message));
   bool request_success = NetUtils::makeRequest(ss, url, formParts);
   
   // An error message should have been shown previous to this point.
   if (!request_success)
      return false;
   
   // Parse response from Facebook.
   Json::Value root;
   Json::Reader reader;
   
   bool parsingSuccessful = reader.parse(ss.str(), root);
   
   // Make sure no parsing errors occurred.
   if (!parsingSuccessful)
   {
      cout << "Failed to parse the document." << endl;
      return false;
   }
   
   // Show any errors reported by Facebook.
   int error_code = NetUtils::showErrorMessage(root);
   if (error_code)
      return false;
   
   ID = root["id"].asString();
   return true;
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
   cout << "Usage: facebash [options]"                                              << endl;
   cout                                                                             << endl;
   cout << "Options:"                                                               << endl;
   cout << "   -c, --comment    Adds comment to the nth most recent post."          << endl;
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
   cout << "   -u, --upload     Batch uploads photos and videos to Facebook."       << endl;
   cout << "   -v, --version    Displays version information"                       << endl;
   cout                                                                             << endl;
}

/*
 * Show Version Information:
 * Displays the program's current version and copyright information.
 */
void ArgParse::ShowVersion()
{
   cout << PROG << " v" << PKGVERSION               << endl;
   cout << "Copyright (c) 2012 Christopher J. Wald" << endl;
}

/*
 * Parse Number Range:
 * Parses a range of numbers. Sets of numbers are comma delimited. A
 * set can be one number, a fixed span, or an arbitrary span. A fixed
 * span is two '-' seperated numbers where each number is the range is
 * added to the list inclusivly. An arbitrary span is one number with
 * a '*' either before or after it. If the '*' is before the number,
 * then all numbers between 0 and the following number inclusively are
 * added to the list. If the '*' is after the number, all numbers
 * between the number and Journal::MAX_SIZE inclusivly are added to
 * the list.
 */
bool ArgParse::parseRange(const string & argument, set<int> & list)
{
   // Assign this to a local variable so I can use an iterator.
   string arg = argument;

   // Use a set to store the indicies so duplicates will be
   // automatically ignored.
   set<int> indices;

   // Set up the unit variable to hold the number being parsed.
   const string DEFAULT_UNIT = "";
   string unit = DEFAULT_UNIT;

   // Have some memory of previous entries. This is useful for
   // creating ranges.
   int lasti = 0;
   int thisi = 0;

   // Decide whether the previous number should be applied to a
   // range.
   bool dorange = false;

   // Finally parse through the entire argument.
   string::iterator itr;
   for (itr = arg.begin(); itr != arg.end(); itr ++)
   {
      // If the current character is a comma update lasti and thisi
      // and insert numbers inclusivly over a range if applicable.
      if (*itr == ',')
      {
	 lasti = thisi;
	 thisi = atoi(unit.c_str()) - 1;
	 if (dorange)
	 {
	    for (int i = lasti; i <= thisi; i ++)
	    {
	       if (i >= 0)
		  indices.insert(i);
	    }
	    dorange = false;
	 }
	 else
	 {
	    if (thisi >= 0)
	       indices.insert(thisi);
	 }
	 unit = DEFAULT_UNIT;
      }

      // If the current character is a dash just update lasti and
      // thisi, signal that a range is being entered and reset unit.
      else if (*itr == '-')
      {
	 lasti = thisi;
	 thisi = atoi(unit.c_str()) - 1;
	 unit = DEFAULT_UNIT;
	 dorange = true;
      }

      // If the current character is an asterisk check to see if it is
      // before a number or after. If it is before signal for a range
      // starting at zero. If it is after do a range from the previous
      // number to Journal::MAX_SIZE and reset everything.
      else if (*itr == '*')
      {
	 if (unit == DEFAULT_UNIT)
	 {
	    lasti = thisi;
	    thisi = 0;
	    dorange = true;
	 }
	 else
	 {
	    lasti = atoi(unit.c_str()) - 1;
	    thisi = Journal::MAX_SIZE;
	    for (int i = lasti; i <= thisi; i ++)
	    {
	       if (i >= 0)
		  indices.insert(i);
	    }
	    unit = DEFAULT_UNIT;
	 }
      }

      // If the current character is a number add it to the end of
      // unit.
      else if (*itr >= '0' && *itr <= '9')
      {
	 unit += *itr;
      }

      // If the current character is something else alert the user and
      // stop execution, returning false.
      else
      {
	 cout << "Parser error. Unknown character '" << *itr << "'" << endl;
	 return false;
      }
   }

   // Now clean up anything left at the end. Do a range if needed or
   // just add the last number to the list.
   if (unit.length() > 0)
   {
      lasti = thisi;
      thisi = atoi(unit.c_str()) - 1;
      if (dorange)
      {
	 for (int i = lasti; i <= thisi; i ++)
	 {
	    if (i >= 0)
	       indices.insert(i);
	 }
	 dorange = false;
      }
      else
      {
	 if (thisi >= 0)
	    indices.insert(thisi);
      }
   }

   list = indices;

   // Opperation completed successfully.
   return true;
}
