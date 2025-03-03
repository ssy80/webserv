#ifndef RESHELPER_HPP
# define RESHELPER_HPP

#include "AServer.hpp"
#include "Response.hpp"
#include "Request.hpp"
#include "ConfigLocation.hpp"
#include "MIME.hpp"
#include <sys/socket.h>
#include <dirent.h>

vector<unsigned char> readRequestFile(const string& resource);
vector<unsigned char> readRequestCGI(const string& scriptPath);
string getFileExtension(const string& filename);
string filetype(const string& url);
string getHandler(Request req, ConfigLocation config);
string postHandler(Request req, ConfigLocation config);
string deleteHandler(Request req, ConfigLocation config);
string otherHandler();
string listdir(const string& path);

#endif