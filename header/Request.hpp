#pragma once
#ifndef __REQUEST_H__
#define __REQUEST_H__

#include <iostream>
#include <string>
#include <map>
#include <sstream>
#include <vector>
#include "AServer.hpp"

using std::map;
using std::vector;

class Request {

	public:
		string method;
		string url;
		string version;
		map<string, string> headers;
		string body;

		Request();
		~Request();
		void print();
};

#endif