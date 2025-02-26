#pragma once
#ifndef __REQSERVER_HPP
#define __REQSERVER_HPP

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "AServer.hpp"
#include "Request.hpp"
#include "RequestParser.hpp"

#define PORT 8080

class ReqServer : public AServer{
	~ReqServer();
	void startServer();
};

#endif