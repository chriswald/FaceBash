#ifndef __NOTIF_LOG_HPP
#define __NOTIF_LOG_HPP

#include <iostream>
#include <string>
#include <sstream>

#include <json/json.h>

#include "NetUtils.hpp"
#include "Notification.hpp"

using std::endl;
using std::ostream;
using std::string;
using std::stringstream;

class NotifLog
{
public:
   NotifLog(bool plain_format);
   ~NotifLog();

   bool getNotifications();
   unsigned int length();

   friend ostream & operator<<(ostream & os, const NotifLog & log);

private:
   bool plain;

   unsigned int Length;
   Notification* notifs;
};

#endif //__NOTIF_LOG_HPP
