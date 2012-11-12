#include "Comment.h"

Comment::Comment(Json::Value value, int i)
{
   val = value;
   index = i;
}

int Comment::getNumLikes() const
{
   return val["likes"].asInt();
}

string Comment::getPostedBy() const
{
   return val["from"]["name"].asString();
}

string Comment::getText() const
{
   return val["message"].asString();
}

int Comment::getIndex() const
{
   return index;
}

void Comment::Like()
{
   stringstream ss;
   string url = string("https://graph.facebook.com/") + val["id"].asString() + string("/likes");
   NetUtils::postRequest(ss, url);
}
