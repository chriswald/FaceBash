#include "NewsStory.h"

NewsStory::NewsStory(const Json::Value & news_story, unsigned int i)
{
  story = news_story;
  index = i;
  ID = news_story["id"].asString();
}

ostream & operator<<(ostream & os, const NewsStory & news)
{
  stringstream ss;
  news.formatNewsStory(ss); 
  os << ss.str();
  return os;
}

string NewsStory::getID()
{
  return ID;
}


/*
 * Format News Story:
 * Formats a news story by printing a nice border, the poster, and the
 * message. The message is formatted for width.
 */
void NewsStory::formatNewsStory(stringstream & ss) const
{
  string message = story["message"].asString();
  string posted_by = story["from"]["name"].asString();
  
  // This conditional is probably unnecessarry since the same
  // condition is being checked in ShowNewsFeed, but its also not
  // hurting anything so I'll keep it.
  if (strlen(message.c_str()) > 0)
    {
      vector<string> lines;
      string remainder = message;
      
      vector<string> new_lines;
      // Break the message body at newlines.
      int nl_index = remainder.find("\n");
      while (nl_index != -1 && nl_index != (int) strlen(remainder.c_str()))
	{
	  string line = remainder.substr(0, nl_index);
	  remainder = remainder.substr(nl_index + 1);
	  new_lines.push_back(line);
	  nl_index = remainder.find("\n");
	}
      new_lines.push_back(remainder);
      
      // Run through each of those lines and make sure it does not
      // exceed LINE_WIDTH. Split it into sub lines until it does.
      for (unsigned int j = 0; j < new_lines.size(); j ++)
	{
	  string new_line = new_lines[j];
	  while (strlen(new_line.c_str()) > LINE_WIDTH)
	    {
	      int space_index = new_line.substr(0, LINE_WIDTH).rfind(" ");
	      string line = new_line.substr(0, space_index);
	      new_line = new_line.substr(space_index + 1);
	      lines.push_back(line);
	    }
	  lines.push_back(new_line);
	}
      
      // Write each individual portion of the story.
      writeSeperatorLine(ss);
      writeNameLine(ss, posted_by);
      writeSeperatorLine(ss);
      writeMessageLines(ss, lines);
      writeSeperatorLine(ss);
      ss << endl;
    }
}

/*
 * Write Message Line:
 * Writes each line from lines to os with leading characters and
 * trailing characters with enough spaces to evenly align everything.
 */
void NewsStory::writeMessageLines(stringstream & ss, const vector<string> lines) const
{
  stringstream tmp;
  for (unsigned int i = 0; i < lines.size(); i ++)
    {
      stringstream stmp;
      stmp << "|  " << lines[i];
      int len = strlen(stmp.str().c_str());
      for (unsigned int j = len; j < LINE_WIDTH+5; j ++)
	stmp << " ";
      stmp << "|" << endl;
      tmp << stmp.str();
    }
  ss << tmp.str();
}

/*
 * Write Name Line:
 * Writes the name (left aligned) and index (right aligned) to os. The
 * two values are written with a fixed width of LINE_WIDTH. Leading
 * and trailing characters are also printed.
 */
void NewsStory::writeNameLine(stringstream & ss, const string & name) const
{
  stringstream tmp;
  stringstream sindex;
  sindex << index << " ";
  tmp << "| " << name.c_str();

  int name_len = strlen(tmp.str().c_str());
  int indx_len = strlen(sindex.str().c_str());

  for (unsigned int i = name_len; i < LINE_WIDTH+5 - indx_len; i ++)
    tmp << " ";
  tmp << sindex.str() << "|" << endl;
  
  ss << tmp.str();
}
  
/*
 * Write Seperator Line:
 * Writes a line of width LINE_WIDTH used as a filler / seperator
 * between elements.
 */
void NewsStory::writeSeperatorLine(stringstream & ss) const
{
  stringstream tmp;

  tmp << "|";
  for (unsigned int i = 1; i < LINE_WIDTH+5; i ++)
    tmp << "-";
  tmp << "|" << endl;

  ss << tmp.str();
}    
