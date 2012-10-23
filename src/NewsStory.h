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

#include "Comment.h"
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
  string getID() const;

  void CommentOnStory(const string & message);

 private:
  void formatNewsStory(stringstream & ss) const;
  vector<string> setLineWidth(string message, int width) const;
  void writeSeperatorLine(stringstream & ss) const;
  void writeNameLine(stringstream & ss, const string & name) const;
  void writeMessageLines(stringstream & ss, const vector<string> lines) const;
  void writeCommentPostedBy(stringstream & ss, const Comment & c) const;
  void writeCommentLines(stringstream & ss, vector<string> lines) const;
  void writeCommentSeperatorLine(stringstream & ss) const;

  void get_num_likes();
  void get_comments();

  const static unsigned int LINE_WIDTH = 70;

  Json::Value story;
  int index;
  string ID;

  int num_likes;
  int num_comments;
  vector<Comment> comments;
};

#endif // __NEWS_STORY_H
