#include "Comment.h"

Comment::Comment(Json::Value value)
{
   val = value;
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
