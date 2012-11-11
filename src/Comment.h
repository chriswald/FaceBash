#ifndef __COMMENT_H
#define __COMMENT_H

#include <cstring>

#include <json/json.h>

using std::string;

class Comment
{
public:
   Comment (Json::Value);
   int getNumLikes() const;
   string getPostedBy() const;
   string getText() const;
   
private:
   Json::Value val;
};

#endif // __COMMENT_H
