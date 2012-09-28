#ifndef __ARG_PARSE_H
#define __ARG_PARSE_H

#include <cstring>
#include <iostream>

using std::cin;
using std::cout;
using std::endl;
using std::istream;
using std::ostream;
using std::string;

class ArgParse
{
 public:
  ArgParse (int argc, char *argv[]);
  ~ArgParse();

  void parseArgs();
  bool argvHas(string arg);

  void Login();
  void UpdateStatus();
  void ShowNewsFeed();
  void AboutFriend();
  void ShowUpcomingBirthdays();
  void AboutMe();

 private:
  string prompt(string message, ostream os = cout, istream is = cin);

  int count;
  char **args;
};

#endif // __ARG_PARSE_H
