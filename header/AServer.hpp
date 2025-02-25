#pragma once
#ifndef __ASERVER_H__
#define __ASERVER_H__

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