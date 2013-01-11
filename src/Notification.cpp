#include "Notification.hpp"

Notification::Notification()
{
   title = string("");
}

Notification::Notification(const string & text)
{
   title = text;
}

void Notification::setTitle(const string & text)
{
   title = text;
}

void Notification::setID(const string & id)
{
   ID = id;
}

string Notification::getID()
{
   return ID;
}

ostream & operator<<(ostream & os, const Notification & notification)
{
   os << notification.title;
   return os;
}
