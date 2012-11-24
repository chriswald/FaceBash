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
   const static int MAX_SIZE = 25;

   Journal(bool get_stories_now = false, string who = string("\0"));
   ~Journal();
   friend ostream & operator<<(ostream & os, const Journal j);
   int length();
   NewsStory & operator[](int index);
   bool getNewsStories(const string & who);
   
private:
   vector<NewsStory> news_stories;
};

#endif // __JOURNAL_H
