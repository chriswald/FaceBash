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
