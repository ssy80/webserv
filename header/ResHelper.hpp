#ifndef RESHELPER_HPP
# define RESHELPER_HPP

#include "AServer.hpp"
#include "Response.hpp"
#include "Request.hpp"
#include "MIME.hpp"
#include <sys/socket.h>

vector<unsigned char> readFile(const string& resource);
void sendRes(int client_socket, const string& output);
string getFileExtension(const string& filename);
string filetype(const string& url);
void getHandler(int client_socket, Request req);
void otherHandler(int client_socket, Request req);

#endif