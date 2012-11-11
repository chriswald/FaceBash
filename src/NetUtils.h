#ifndef __NET_UTILS_H
#define __NET_UTILS_H

#include <fstream>
#include <iostream>
#include <sstream>
#include <cstring>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Easy.hpp>

#include <json/json.h>

using std::cout;
using std::endl;
using std::ifstream;
using std::string;
using std::stringstream;

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
   
   static int showErrorMessage(const Json::Value & root);
   
private:
   static string authToken();
};

#endif // __NET_UTILS_H
