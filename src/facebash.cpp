#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
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

	  int status;
	  pid_t pid = fork();

	  if (pid < 0)
	    {
	      perror("fork error");
	    }
	  else if (pid == 0)
	    {
	      execl("/usr/bin/python2.7", "/usr/bin/python2.7", "scripts/login.py", login.user().c_str(), login.pass().c_str(), (char *) 0);
	    }
	  else
	    {
	      if ((pid = wait(&status)) == -1)
		perror("wait error");
	    }

	  std::ifstream member27;
	  member27.open("/home/pi/.facebash/member27");
	  if (member27.is_open())
	    {
	      while (member27.good())
		{
		  std::string line;
		  getline (member27, line);
		  std::cout << line << std::endl;
		}
	      member27.close();
	    }
	  else
	    {
	      std::cerr << "Unable to login." << std::endl;
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
