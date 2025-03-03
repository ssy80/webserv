#ifndef RESHELPER_HPP
# define RESHELPER_HPP

#include "AServer.hpp"
#include "Response.hpp"
#include "Request.hpp"
#include "MIME.hpp"
#include <sys/socket.h>
#include <dirent.h>

vector<unsigned char> readFile(const string& resource);
void sendRes(int client_socket, const string& output);
string getFileExtension(const string& filename);
string filetype(const string& url);
void getHandler(int client_socket, Request req, const string& root, bool idx=false);
void postHandler(int client_socket, Request req);
void deleteHandler(int client_socket, Request req);
void otherHandler(int client_socket, Request req);
string listdir(const string& path);


#endif