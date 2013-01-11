#include "Comment.hpp"

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
   string url = string("https://graph.facebook.com/")
              + val["id"].asString()
              + string("/likes");
   bool request_success = NetUtils::postRequest(ss, url);

   if (!request_success)
      return;

   if (ss.str() != "true")
   {
      // Make sure to display any errors that Facebook may have given us.
      Json::Value root;
      Json::Reader reader;
      bool parsingSuccessful = reader.parse(ss.str(), root);
      if (!parsingSuccessful)
      {
	 std::cerr << "Failed to parse response." << endl;
	 std::cerr << ss.str() << endl;
      }
   
      NetUtils::showErrorMessage(root);
   }
}
