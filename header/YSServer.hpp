#pragma once
#ifndef __YSSERVER_HPP
#define __YSSERVER_HPP

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "AServer.hpp"
#include "Request.hpp"
#include "RequestParser.hpp"

#define PORTYS 8080

class YSServer : public AServer{
	~YSServer();
	void startServer();
};

#endif