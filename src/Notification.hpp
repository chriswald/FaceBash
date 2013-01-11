#ifndef __NOTIFICATION_HPP
#define __NOTIFICATION_HPP

#include <iostream>
#include <string>

using std::ostream;
using std::string;

class Notification
{
public:
   Notification();
   Notification(const string & title);

   void setTitle(const string & title);
   void setID(const string & id);

   string getID();

   friend ostream & operator<<(ostream & os, const Notification & notification);

private:
   string ID;
   string title;
};

#endif //__NOTIFICATION_HPP
