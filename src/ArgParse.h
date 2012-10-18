#ifndef __ARG_PARSE_H
#define __ARG_PARSE_H

#include <cstdio>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Easy.hpp>

#include <json/json.h>

#include "LoginField.h"
#include "NewsStory.h"

using std::cerr;
using std::cin;
using std::cout;
using std::endl;
using std::ifstream;
using std::ostream;
using std::ostringstream;
using std::setfill;
using std::setw;
using std::stack;
using std::string;
using std::stringstream;
using std::vector;

const string version = ".1";

class ArgParse
{
 public:
  ArgParse (int argc, char **argv);
  ~ArgParse();

  void ParseArgs();
  bool argHas(string arg);

  void Comment();
  void Login();
  void Logout();
  void UpdateStatus();
  void ShowNewsFeed();
  void AboutFriend();
  void ShowUpcomingBirthdays();
  void AboutMe();

  void ShowHelpText();
  void ShowVersion();

 private:
  string prompt(string message);
  string authToken();
  string getFriendID(string name);
  void   formatNewsStory(ostream & os, Json::Value story, int index);
  int    showErrorMessage(const Json::Value & root);
  bool   relogin();

  bool makeRequest(stringstream & ss, const string & url);
  bool makeRequest(stringstream & ss, const string & url, const cURLpp::Forms & formParts);

  int     count;
  string *arguments;

  const static unsigned short int LINE_WIDTH = 70;
  const static unsigned short int ERROR_CODE_LENGTH = 3;
};

#endif // __ARG_PARSE_H
