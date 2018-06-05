#include "network_interface.h"
#include <string>
using namespace std;

ServerInfo g_serverinfo;

int main(int argc, char **argv){

    CConfigFileReader conf("./config.cfg");
	map<string,string>confmap=conf.GetConfigMap();
	string listenIp=confmap["listenIP"];
	string port    =confmap["port"];

	
	g_serverinfo.ipaddr=listenIp;
	g_serverinfo.port  =(int)atoi(port.c_str());
   	
	//conf._LoadFile(const char * filename)

	NETWORK_INTERFACE->run();
	return 0;
}
