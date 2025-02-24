#include "src/server/AServer.hpp"
#include "src/server/Server.hpp"

int main(int argc, char **argv){
	if (argc > 2){
		cout << "only 0 or 1 agrument is allowed" << endl;
		return 0;
	}
	AServer* server;
	if (argc == 2){
		string tmpstr = argv[1];
		server = new Server(tmpstr);
	}
	else
		server = new Server();
	server->start(); 
	delete server;
	return 0;
}

// c++ main.cpp ./src/server/*.cpp -I ./src/server/ 