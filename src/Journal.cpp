#include "Journal.hpp"

Journal::Journal(bool get_stories_now, string who)
{
   plain_format = false;
   if (get_stories_now)
      getNewsStories(who);
}

Journal::~Journal()
{}

ostream & operator<<(ostream & os, const Journal j)
{
   for (int i = j.news_stories.size()-1; i >= 0; i --)
   {
      os << j.news_stories[i];
   }
   return os;
}

int Journal::length()
{
   return news_stories.size();
}

NewsStory & Journal::operator[](int index)
{
   NewsStory & tmp = news_stories[index];
   return tmp;
}

bool Journal::getNewsStories(const string & who)
{
   // Get the JSON from Facebook
   stringstream ss;
   string url = string("https://graph.facebook.com/");
   if (who == "\0")
      url += string("me/home");
   else
      url += who + string("/feed");

   bool request_success = NetUtils::makeRequest(ss, url);

   // If the request wasn't successfully made just return. Some
   // function previous to this should have displayed some error
   // message.
   if (!request_success)
   {
      return false;
   }
   
   // Parse the JSON
   Json::Value root;
   Json::Reader reader;
   
   bool parsingSuccessful = reader.parse(ss.str(), root);
   if (!parsingSuccessful)
   {
      cout << "Failed to parse the document." << endl;
      return false;
   }
   
   if (NetUtils::showErrorMessage(root))
   {
      return false;
   }
   
   const Json::Value posts = root["data"];
   
   unsigned int how_many = 0;
   for (unsigned int i = 0; i < posts.size(); i ++)
   {
      news_stories.push_back(NewsStory(posts[i], ++how_many, plain_format));
   }
   
   return true;
}

void Journal::setFormatStyle(bool plain)
{
   plain_format = plain;
}
