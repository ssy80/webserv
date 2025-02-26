#pragma once
#ifndef __ASERVER_HPP
#define __ASERVER_HPP

#include <iostream>
#include <string>

using std::cout;
using std::endl;
using std::string;

// abstract class
class AServer{
	public:
		AServer();
		virtual ~AServer();
		virtual void startServer()=0;
};

#endif