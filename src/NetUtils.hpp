#ifndef __NET_UTILS_HPP
#define __NET_UTILS_HPP

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Easy.hpp>

#include <json/json.h>

#include "Utils.hpp"

using std::cout;
using std::endl;
using std::ifstream;
using std::map;
using std::string;
using std::stringstream;
using std::transform;

class NetUtils
{
public:
   static bool makeRequest(stringstream & ss,
			   const string & url);
   
   static bool makeRequest(stringstream & ss,
			   const string & url,
			   const cURLpp::Forms & formParts);
   
   static bool makeRequest(stringstream & ss,
			   const string & url,
			   const string & args);

   static bool postRequest(stringstream & ss,
			   const string & url);
   
   static int showErrorMessage(const Json::Value & root);

   static bool getFriendID(const string & name, string & ID);
   
private:
   static string authToken();
   static bool queryForFriends(Json::Value & root);
   static void searchForName(const Json::Value & root,
			     map<string, string> & ids,
			     const string & name);
};

#endif // __NET_UTILS_HPP
