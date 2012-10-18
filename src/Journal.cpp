#include "Journal.h"

Journal::Journal(bool get_stories_now)
{
  if (get_stories_now)
    getNewsStories();
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

bool Journal::getNewsStories()
{

  // Get the JSON from Facebook
  stringstream ss;
  string url = string("https://graph.facebook.com/me/home");
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
      string content_type = posts[i]["type"].asString();
      
      // Make sure the post is a status and isn't empty
      if (strcmp(content_type.c_str(), "status") == 0 &&
	  strlen(posts[i]["message"].asString().c_str()) > 0)
	{
	  news_stories.push_back(NewsStory(posts[i], ++how_many));
	}
    }

  return true;
}
