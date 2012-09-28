//These probably aren't needed anymore
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

#include "ArgParse.h"
#include "LoginField.h"

const std::string AppId = "103806206443210";

int main(int argc, char* argv[])
{
  ArgParse argparse = ArgParse(argc, argv);
  argparse.parseArgs();


  /*for (int i = 0; i < argc; i ++)
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
	}*/

  return 0;
}
