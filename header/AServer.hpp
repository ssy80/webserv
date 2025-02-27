#pragma once
#ifndef __ASERVER_HPP
#define __ASERVER_HPP

#include <iostream>
#include <string>
#include <fstream>

using std::cout;
using std::endl;
using std::string;
using std::ifstream;
using std::ios;
using std::noskipws;

// abstract class
class AServer{
	public:
		AServer();
		virtual ~AServer();
		virtual void startServer()=0;
};

#endif