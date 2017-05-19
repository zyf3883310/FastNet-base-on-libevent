#include <iostream>
#include "ServerCfg.h"
#include "FastNetServer.h"
using namespace std;
using namespace fastnet;
int main(int argc, char *argv[])
{
	cout << "main begin" << endl;
	ServerCfg cfg;
	CFastNetServer server;
	server.InitNetServerCfg(&cfg, NULL, NULL);
	server.Start();
	server.Stop();
	cout << "main end\n";
	return 0;
}
