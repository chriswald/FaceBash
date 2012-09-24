#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>

#include "LoginField.h"

const std::string AppId = "103806206443210";

int main(int argc, char* argv[])
{
  for (int i = 0; i < argc; i ++)
    {
      if (strcmp(argv[i], "-g") == 0 && i != argc-1)
	{
	  try
	    {
	      std::string ID = std::string(argv[i+1]);
	      std::string full_url = std::string("https://graph.facebook.com/") + ID;
	      curlpp::options::Url url(full_url);
	      curlpp::Easy request;
	      request.setOpt(url);
	      request.perform();
	      std::cout << std::endl << std::flush;
	    }
	  catch (curlpp::RuntimeError &e)
	    {
	      std::cerr << "E: " << e.what() << std::endl;
	    }
	  catch (curlpp::LogicError &e)
	    {
	      std::cerr << "E: " << e.what() << std::endl;
	    }
	}
      if (strcmp(argv[i], "-l") == 0)
	{
	  LoginField login = LoginField();
	  login.readUser("Email: ");
	  login.readPass();

	  pid_t child = 0;
	  child = fork();
	  if (child < 0)
	    {
	      std::cerr << "Could not branch to browser." << std::endl;
	      std::cerr << "Ensure links is installed." << std::endl;
	    }
	  else if (child == 0)
	    {
	      bool found_token = true; // false;
	      while (!found_token)
		{
		  // Poll for token
		}
	    }
	  else
	    {
	      execl("/usr/bin/python2.7", "/usr/bin/python2.7", "login.py", login.user().c_str(), login.pass().c_str(), (char *) 0);
	    }
	}

      if (strcmp(argv[i], "-u") == 0 && i != argc-1)
	{
	  try
	    {
	      curlpp::Cleanup cleanup;
	      curlpp::options::Url url(std::string(argv[i+1]));
	      curlpp::Easy request;

	      request.setOpt(url);

	      request.perform();
	      std::cout << std::endl << std::flush;
	    }
	  catch (curlpp::RuntimeError &e)
	    {
	      std::cout << e.what() << std::endl;
	    }
	  catch (curlpp::LogicError &e)
	    {
	      std::cout << e.what() << std::endl;
	    }
	}
    }

  return 0;
}
