//These probably aren't needed anymore
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

//#include <curlpp/cURLpp.hpp>
//#include <curlpp/Easy.hpp>
//#include <curlpp/Options.hpp>

#include "ArgParse.h"
#include "LoginField.h"

const std::string AppId = "103806206443210";

int main(int argc, char* argv[])
{
  ArgParse parse = ArgParse(argc, argv);
  parse.ParseArgs();

  return 0;
}
