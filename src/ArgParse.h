#ifndef __ARG_PARSE_H
#define __ARG_PARSE_H

#include <cstring>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Easy.hpp>

#include "LoginField.h"

using std::cerr;
using std::cin;
using std::cout;
using std::endl;
using std::ifstream;
using std::istream;
using std::ostream;
using std::string;

class ArgParse
{
 public:
  ArgParse (int argc, char **argv);
  ~ArgParse();

  void ParseArgs();
  bool argHas(string arg);

  void Login();
  void UpdateStatus();
  void ShowNewsFeed();
  void AboutFriend();
  void ShowUpcomingBirthdays();
  void AboutMe();

 private:
  string prompt(string message);
  string authToken();

  int count;
  string *argument;
};

#endif // __ARG_PARSE_H
