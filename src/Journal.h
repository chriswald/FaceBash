#ifndef __JOURNAL_H
#define __JOURNAL_H

#include <iostream>
#include <string>
#include <vector>

#include "NetUtils.h"
#include "NewsStory.h"

using std::ostream;
using std::string;
using std::vector;

class Journal
{
public:
   Journal(bool get_stories_now = false, string who = string("me"));
   ~Journal();
   friend ostream & operator<<(ostream & os, const Journal j);
   int length();
   NewsStory operator[](int index);
   
private:
   bool getNewsStories(string who);
   vector<NewsStory> news_stories;
};

#endif // __JOURNAL_H
