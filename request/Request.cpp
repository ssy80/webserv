#include "../header/Request.hpp"

Request::Request(): 
method(""),
url(""),
version(""),
body("")
{};

Request::~Request()
{};

void Request::print() {
	cout << "Method: " << method << endl;
	cout << "URL: " << url << endl;
	cout << "Version: " << version << endl;
	cout << "Headers: " << endl;
	for (map<string, string>::iterator it = headers.begin(); it != headers.end(); ++it)
		cout << "  " << it->first << ": " << it->second << endl;
	cout << "Body: " << body << endl;
}