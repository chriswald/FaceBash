#ifndef __ARG_PARSE_HPP
#define __ARG_PARSE_HPP

#include <cctype>
#include <cstdio>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <set>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Easy.hpp>

#include <json/json.h>

#include "Journal.hpp"
#include "LoginField.hpp"
#include "NetUtils.hpp"
#include "NewsStory.hpp"
#include "NotifLog.hpp"
#include "Utils.hpp"

using std::cin;
using std::cout;
using std::endl;
using std::ifstream;
using std::ostream;
using std::ostringstream;
using std::set;
using std::setfill;
using std::setw;
using std::stack;
using std::string;
using std::stringstream;
using std::vector;

const string version = ".3";

class ArgParse
{
public:
   ArgParse (int argc, char **argv);
   ~ArgParse();
   
   void ParseArgs();
   bool argHas(string arg);
   int  argIndex(string arg);
   bool isCommand(string arg);

   void ShowNotifications();
   void Comment();
   void Like();
   void Login();
   void Logout();
   void ShowNewsFeed();
   void UpdateStatus();
   void Upload();
   void AboutFriend();
   void ShowUpcomingBirthdays();
   void AboutMe();
   
   void ShowHelpText();
   void ShowVersion();
   
private:
   bool   relogin();
   bool   makeAlbum(const string & name, const string & message, string & ID);
   bool   getAlbumNames(map<string, string> & names);
   bool   sendImagesToAlbum(const string & ID, const vector<string> & files);
   bool   sendVideosToFacebook(const vector<string> & files);

   bool   parseRange(const string & arg, set<int> & list);

   void   addCommand(const string & command, int times);

   int     count;
   string *arguments;
   int     cmdcount;
   vector<string> commands;
   
   const static unsigned short int ERROR_CODE_LENGTH = 3;
};

#endif // __ARG_PARSE_HPP
