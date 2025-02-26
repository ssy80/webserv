#pragma once
#ifndef __REQUESTPARSER_HPP
#define __REQUESTPARSER_HPP

#include "Request.hpp"

using std::istringstream;
using std::getline;
using std::ostringstream;

class RequestParser{
	public:
		static Request parseRequest(const string& rawRequest);
	
	private:
		static void parseMultipartFormData(istringstream& stream, Request& request, const string& contentType);
		static void processFormPart(const string& part, Request& request);
		static string extractFilename(const string& contentDisposition);
		static string extractFieldName(const string& contentDisposition);
};

#endif