#pragma once
#ifndef __REQUEST_HPP
#define __REQUEST_HPP

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
    map<string, string> formFields;  // For non-file form fields
    map<string, string> files;       // For file contents (filename -> file content)

		Request();
		~Request();
		void print();
};

#endif