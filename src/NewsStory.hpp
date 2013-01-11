#ifndef __NEWS_STORY_HPP
#define __NEWS_STORY_HPP

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <json/json.h>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Easy.hpp>

#include "Comment.hpp"
#include "NetUtils.hpp"
#include "utf8.h"

using std::endl;
using std::ostream;
using std::setfill;
using std::setw;
using std::stringstream;
using std::string;
using std::vector;

class NewsStory
{
public:
   NewsStory(const Json::Value & news_story, unsigned int i, bool plain);
   friend ostream & operator<<(ostream & os, const NewsStory & news);
   string getID() const;
   
   void CommentOnStory(const string & message);
   void LikeStory();
   void LikeComment(int index);

   int numComments();

private:
   void formatNewsStory(stringstream & ss) const;
   vector<string> setLineWidth(const string & message, unsigned int width) const;
   void writePostNameLine(stringstream & ss, const string & name) const;
   void writePostMessageLines(stringstream & ss, const vector<string> & lines) const;
   void writePostSeperatorLine(stringstream & ss) const;
   void writeCommentNameLine(stringstream & ss, const Comment & c) const;
   void writeCommentMessageLines(stringstream & ss, const vector<string> & lines) const;
   void writeCommentSeperatorLine(stringstream & ss) const;

   void writeNameLine(stringstream & ss,
		      const string & name,
		      const string & prefix,
		      int likes,
		      const string & index) const;
   
   void writeMessageLines(stringstream & ss,
			  const vector<string> & lines,
			  const string & prefix) const;
   void writeSeperatorLine(stringstream & ss,
			   const string & prefix) const;

   void formatOnStoryType(string & message) const;

   void get_num_likes();
   void get_comments();

   size_t linelength(const string & str) const;

   const static unsigned short int LINE_WIDTH = 70;
   
   bool plain_format;

   Json::Value story;
   int index;
   string ID;
   
   int num_likes;
   int num_comments;
   vector<Comment> comments;
};

#endif // __NEWS_STORY_HPP
