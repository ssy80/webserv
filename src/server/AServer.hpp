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
		// default constructor when there is no argument
		AServer();
		// when dir of config is given
		AServer(const string& config);
		virtual ~AServer();
		virtual void start()=0;
};

#endif