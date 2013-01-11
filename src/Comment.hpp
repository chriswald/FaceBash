#ifndef __COMMENT_HPP
#define __COMMENT_HPP

#include <string>

#include <json/json.h>

#include "NetUtils.hpp"

using std::string;
using std::stringstream;

class Comment
{
public:
   Comment (Json::Value value, int index);
   int getNumLikes() const;
   string getPostedBy() const;
   string getText() const;
   int getIndex() const;

   void Like();
   
private:
   Json::Value val;
   int index;
};

#endif // __COMMENT_HPP
