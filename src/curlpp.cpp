#include <iostream>
#include <string.h>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>

using std::cout;
using std::cerr;
using std::endl;

int main(int argc, char* argv[])
{
  if (argc != 3)
    {
      cerr << "Usage: -u <url>" << endl;
      return -1;
    }

  for (int i = 0; i < argc; i ++)
    {
      if (strcmp(argv[i], "-u") == 0 && i != argc-1)
	{
	  try
	    {
	      curlpp::Cleanup cleanup;
	      curlpp::options::Url url(std::string(argv[i+1]));
	      curlpp::Easy request;

	      request.setOpt(url);

	      request.perform();
	    }
	  catch (curlpp::RuntimeError &e)
	    {
	      cout << e.what() << endl;
	    }
	  catch (curlpp::LogicError &e)
	    {
	      cout << e.what() << endl;
	    }
	}
    }

  return 0;
}
