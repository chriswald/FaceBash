#include "NewsStory.h"

/*
 * Creates a new NewsStory object and sets its ID, number of likes, and number
 * of comments.
 */
NewsStory::NewsStory(const Json::Value & news_story, unsigned int i)
{
   story = news_story;
   index = i;
   ID = news_story["id"].asString();
   
   get_num_likes();
   get_comments();
}

/*
 * Operator <<:
 * Formats the news story and prints it to ostream os.
 */
ostream & operator<<(ostream & os, const NewsStory & news)
{
   stringstream ss;
   news.formatNewsStory(ss); 
   os << ss.str();
   return os;
}

/*
 * Get ID:
 * Returns the news story's ID.
 */
string NewsStory::getID() const
{
   return ID;
}

/*
 * Comment on Story:
 * Adds the message to the news story as a comment by passing it to Facebook.
 */
void NewsStory::CommentOnStory(const string & message)
{
   cURLpp::Forms form;
   form.push_back(new cURLpp::FormParts::Content("message", message));
   
   string url = string("https://graph.facebook.com/") + ID + string("/comments");
   
   stringstream ss;
   bool request_success = NetUtils::makeRequest(ss, url, form);
   
   if (!request_success)
      return;
   
   Json::Value root;
   Json::Reader reader;
   bool parsing_success = reader.parse(ss.str(), root);
   if (!parsing_success)
   {
      cout << "Failed to parse response." << endl;
      cout << ss.str() << endl;
   }
   
   NetUtils::showErrorMessage(root);
}

void NewsStory::LikeStory()
{
   stringstream ss;
   string url = string("https://graph.facebook.com/") + ID + string("/likes");
   bool request_success = NetUtils::postRequest(ss, url);
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
   if (message.length() > 0)
   {
      
      vector<string> lines = setLineWidth(message, LINE_WIDTH);
      
      // Write each individual portion of the story.
      writeSeperatorLine(ss);
      writeNameLine(ss, posted_by);
      writeSeperatorLine(ss);
      writeMessageLines(ss, lines);
      writeSeperatorLine(ss);

      for (int i = 0; i < num_comments; i ++)
      {
	 Comment c = comments[i];
	 if (c.getText().length() > 0)
	 {
	    vector<string> l = setLineWidth(c.getText(), LINE_WIDTH - 5);
	    writeCommentPostedBy(ss, c);
	    writeCommentLines(ss, l);
	    writeCommentSeperatorLine(ss);
	 }
      }
      
      ss << endl;
   }
}

vector<string> NewsStory::setLineWidth(string message, int width) const
{
   vector<string> lines;
   string remainder = message;
   
   vector<string> new_lines;
   // Break the message body at newlines.
   int nl_index = remainder.find("\n");
   while (nl_index != -1 && nl_index != (int) remainder.length())
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
      while (new_line.length() > (unsigned int) width)
      {
	 int space_index = new_line.substr(0, width).rfind(" ");
	 if (space_index == -1)
	 {
	    string line = new_line.substr(0, width);
	    new_line = new_line.substr(width + 1);
	    lines.push_back(line);
	 }
	 else
	 {
	    string line = new_line.substr(0, space_index);
	    new_line = new_line.substr(space_index + 1);
	    lines.push_back(line);
	 }
      }
      lines.push_back(new_line);
   }
   
   return lines;
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
      int len = stmp.str().length();
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
   
   if (num_likes > 0)
   {
      if (num_likes == 1)
	 tmp << "   <1 Like>";
      else
	 tmp << "   <" << num_likes << " Likes>";
   }
   
   int name_len = tmp.str().length();
   int indx_len = sindex.str().length();
   
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

void NewsStory::writeCommentPostedBy(stringstream & ss, const Comment & c) const
{
   stringstream tmp;
   tmp << "     | " << c.getPostedBy();
   
   if (c.getNumLikes() > 0)
   {
      if (c.getNumLikes() == 1)
	 tmp << "   <1 Like>";
      else
	 tmp << "   <" << c.getNumLikes() << " Likes>";
   }
   
   for (unsigned int i = tmp.str().length(); i < LINE_WIDTH + 5; i ++)
   {
      tmp << " ";
   }
   
   tmp << "|" << endl;
   ss << tmp.str();
}

void NewsStory::writeCommentLines(stringstream & ss, vector<string> lines) const
{
   for (unsigned int i = 0; i < lines.size(); i ++)
   {
      stringstream tmp;
      tmp << "     |   " << lines[i];
      for (unsigned int j = tmp.str().length(); j < LINE_WIDTH + 5; j ++)
	 tmp << " ";
      tmp << "|" << endl;
      ss << tmp.str();
   }
}

void NewsStory::writeCommentSeperatorLine(stringstream & ss) const
{
   stringstream tmp;
   tmp << "     |";
   for (unsigned int i = tmp.str().length(); i < LINE_WIDTH + 5; i ++)
      tmp << "-";
   tmp << "|" << endl;
   ss << tmp.str();
}

void NewsStory::get_num_likes()
{
   num_likes = story["likes"]["count"].asInt();
}

void NewsStory::get_comments()
{
   num_comments = story["comments"]["count"].asInt();
   for (int i = 0; i < num_comments; i ++)
   {
      comments.push_back(Comment(story["comments"]["data"][i]));
   }
}
