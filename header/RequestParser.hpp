#pragma once
#ifndef __REQUESTPARSER_H__
#define __REQUESTPARSER_H__

#include "Request.hpp"

using std::istringstream;
using std::getline;
using std::ostringstream;

class RequestParser{
	public:
		static Request parseRequest(const string& rawRequest);
};

#endif