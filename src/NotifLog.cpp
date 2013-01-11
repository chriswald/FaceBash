#include "NotifLog.hpp"

NotifLog::NotifLog(bool plain_format)
{
   plain = plain_format;
   notifs = NULL;
}

NotifLog::~NotifLog()
{
   delete[] notifs;
   notifs = NULL;
}

bool NotifLog::getNotifications()
{
   stringstream ss;
   string url = "https://graph.facebook.com/me/notifications";
      bool request_success = true;

   request_success = NetUtils::makeRequest(ss, url);

   if (!request_success)
      return false;

   Json::Value root;
   Json::Reader reader;

   bool parsing_successful = reader.parse(ss.str(), root);
   if (!parsing_successful)
   {
      cout << "Failed to parse the document." << endl;
      return false;
   }

   if (NetUtils::showErrorMessage(root))
      return false;

   const Json::Value data = root["data"];
   Length = data.size();
   delete[] notifs;
   notifs = new Notification[Length];

   for (unsigned int i = 0; i < Length; i ++)
   {
      notifs[i] = Notification();
      notifs[i].setTitle(root["data"][i]["title"].asString());
      notifs[i].setID(root["data"][i]["id"].asString());
   }

   return true;
}

unsigned int NotifLog::length()
{
   return Length;
}

ostream & operator<<(ostream & os, const NotifLog & log)
{

   if (log.Length > 0)
   {
      if (!log.plain)
      {
	 os << "Notifications" << endl;
	 os << "=============" << endl;
      }
      for (unsigned int i = 0; i < log.Length; i ++)
	 os << i+1 << ":  " << log.notifs[i] << endl;
   }
   else
   {
      os << "You have no notifications at this time." << endl;
   }

   return os;
}
