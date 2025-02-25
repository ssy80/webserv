#include "../header/Request.hpp"

Request::Request(): 
method(""),
url(""),
version("")
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
	cout << "Form Fields: " << endl;
	for (map<string, string>::iterator it = formFields.begin(); it != formFields.end(); ++it) 
		cout << "  " << it->first << ": " << it->second << endl;
	cout << "Files: " << endl;
	for (map<std::string, string>::iterator it = files.begin(); it != files.end(); ++it) 
		cout << "  " << it->first << ": " << it->second.substr(0, 20) << "..." << endl; // Print part of the file content for debugging
}