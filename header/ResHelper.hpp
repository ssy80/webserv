#ifndef RESHELPER_HPP
# define RESHELPER_HPP

#include "AServer.hpp"
#include "Response.hpp"
#include "Request.hpp"
#include "ConfigLocation.hpp"
#include "ConfigServer.hpp"
#include "MIME.hpp"
#include <sys/socket.h>
#include <dirent.h>

#define TIMEOUT 5000

vector<unsigned char> readRequestFile(const string& resource);
vector<unsigned char> readRequestCGI(const string& scriptPath);
string getFileExtension(const string& filename);
string filetype(const string& url);
string getHandler(Request& req, ConfigServer& configServer, ConfigLocation& configLocation);
string postHandler(Request& req, ConfigServer& configServer, ConfigLocation& configLocation, std::string uploadDirectory);
string deleteHandler(std::string uploadDirectory);
string otherHandler(ConfigServer& configServer);
string listdir(const string& path);

#endif