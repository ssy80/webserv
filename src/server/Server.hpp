#pragma once
#ifndef __SERVER_H__
#define __SERVER_H__

#include "AServer.hpp"

class Server : public AServer{
	public:
		Server();
		Server(const string& config);
		~Server();
		void start();

};

#endif