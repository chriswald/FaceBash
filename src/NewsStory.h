#ifndef __NEWS_STORY_H
#define __NEWS_STORY_H

#include <iostream>
#include <sstream>
#include <cstring>
#include <vector>
#include <json/json.h>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Easy.hpp>

#include "NetUtils.h"

using std::endl;
using std::ostream;
using std::stringstream;
using std::string;
using std::vector;

class NewsStory
{
 public:
  NewsStory(const Json::Value & news_story, unsigned int i);
  friend ostream & operator<<(ostream & os, const NewsStory & news);
  string getID();

  void Comment(const string & message);

 private:
  void formatNewsStory(stringstream & ss) const;  
  void writeSeperatorLine(stringstream & ss) const;
  void writeNameLine(stringstream & ss, const string & name) const;
  void writeMessageLines(stringstream & ss, const vector<string> lines) const;

  const static unsigned int LINE_WIDTH = 70;

  Json::Value story;
  int index;
  string ID;
};

#endif // __NEWS_STORY_H
