#include "Config.h"
#include <unistd.h>
#include <random>

static const int MAXSIZE_HOSTNAME {25}; 

const char* getBBBid()
{
  static char hostname[MAXSIZE_HOSTNAME] = { '\0' };  
  gethostname(hostname, MAXSIZE_HOSTNAME);
  return hostname;
}

const char* getClientID()
{
	std::string clientid {"speak_"}; 
 	std::random_device rd;
 	int result;
 	std::uniform_int_distribution<int> distribution(0,999);
	result = distribution(rd);

	if(result < 100)
	{
		return clientid.append("0").append(std::to_string(result)).c_str();
	}
	else
	{
		return clientid.append(std::to_string(result)).c_str();
	}
}
