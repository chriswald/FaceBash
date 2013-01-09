#include "NetUtils.h"

/*
 * Make Request:
 * Makes a request to the given url and writes the return to
 * ss. Returns true if the request was successful, false otherwise.
 */
bool NetUtils::makeRequest(stringstream & ss, const string & url)
{
   string token = authToken();
   
   if (token == "\0")
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
 * Make Request (With Form data):
 * Makes a request to the given url with the given form data and
 * writes the return to ss. Returns true if the request was
 * successful, false otherwise.
 */
bool NetUtils::makeRequest(stringstream & ss,
			   const string & url,
			   const cURLpp::Forms & formParts)
{
   string token = authToken();
   
   if (token == "\0")
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

/*
 * Make Request:
 * Makes a request to the given url and writes the return to
 * ss. Returns true if the request was successful, false otherwise.
 */
bool NetUtils::makeRequest(stringstream & ss,
			   const string & url,
			   const string & args)
{
   string token = authToken();
   
   if (token == "\0")
   {
      cout << "Please Log in." << endl;
      return false;
   }
   
   cURLpp::Easy request;
   cURLpp::options::Url URL(url + string("?access_token=") + token + string("&") + args);
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

bool NetUtils::postRequest(stringstream & ss,
			   const string & url)
{
   string token = authToken();
   
   if (token == "\0")
   {
      cout << "Please Log in." << endl;
      return false;
   }

   cURLpp::Easy request;
   cURLpp::options::Url URL(url);
   cURLpp::Forms form;
   form.push_back(new cURLpp::FormParts::Content("access_token", token));
   request.setOpt(URL);
   request.setOpt(new cURLpp::Options::HttpPost(form));
   
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
 * Show Error Message:
 * If the Json document contains an error message display it and
 * return true. Return false otherwise.
 */
int NetUtils::showErrorMessage(const Json::Value & root)
{
   string error_message = root["error"]["message"].asString();
   if (error_message != "\0")
   {
      cout << error_message << endl;
      return root["error"]["code"].asInt();
   }
   
   return 0;
}


/*
 * Get Auth Token:
 * Attempts to retrieve the auth token. On success the auth token is
 * returned. On failure \0 is returned.
 */
string NetUtils::authToken()
{
   string token;
   ifstream fin;
   string filename = Utils::authFile();
   fin.open(filename.c_str());
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
bool NetUtils::getFriendID(const string & name, string & ID)
{
   if (name == "me")
   {
      ID = name;
      return true;
   }

   // Query Facebook for a list of all the user's friends.
   Json::Value root;
   if (!queryForFriends(root))
      return false;

   map<string, string> ids;
   
   // Search throught the returned friend list for all names matching
   // the name parameter.
   searchForName(root, ids, name);
   
   // If the number of matches is not exactly one display an error
   // message and return \0. Otherwise return the ID of the one match.
   if (ids.size() > 1)
   {
      cout << "You have " << ids.size() << " friends with '" 
	   << name << "' in their name." << endl;
      map<string, string>::iterator itr = ids.begin();
      int how_many = (ids.size() > 5 ? 5 : ids.size());
      for (int i = 0; i < how_many; i ++, itr ++)
      {
	 cout << itr->first << endl;
      }
      return false;
   }
   else if (ids.size() == 0)
   {
      cout << "You don't have any friends with '" << name 
	   << "' in their name." << endl;
      return false;
   }
   else
   {
      ID = ids.begin()->second;
      return true;
   }
}

bool NetUtils::queryForFriends(Json::Value & root)
{
   stringstream ss;
   string url = string("https://graph.facebook.com/me/friends");
   bool request_success = NetUtils::makeRequest(ss, url);
   
   if (!request_success)
   {
      return false;
   }
   
   // Parse said list.
   Json::Reader reader;
   
   bool parsingSuccessful = reader.parse(ss.str(), root);
   
   // Make sure no errors occurred.
   if (!parsingSuccessful)
   {
      cout << "Failed to parse the document." << endl;
      return false;
   }
   
   int error_code = NetUtils::showErrorMessage(root);
   if (error_code)
   {
      return false;
   }

   return true;
}

void NetUtils::searchForName(const Json::Value & root,
			     map<string, string> & ids,
			     const string & name)
{
   string lname = name;
   transform(name.begin(), name.end(), lname.begin(), tolower);

   const Json::Value data = root["data"];
   for (unsigned int i = 0; i < data.size(); i ++)
   {
      string curName  = data[i]["name"].asString();
      string lcurName = curName;
      string curID    = data[i]["id"].asString();
      transform(curName.begin(), curName.end(), lcurName.begin(), tolower);

      if (lcurName.find(lname.c_str()) < string::npos)
      {
	 ids[curName] = curID;
      }
   }
}
