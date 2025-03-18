/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResHelper.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daong <daong@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/15 09:41:55 by daong             #+#    #+#             */
/*   Updated: 2025/03/15 09:41:56 by daong            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESHELPER_HPP
# define RESHELPER_HPP

#include "AServer.hpp"
#include "Response.hpp"
#include "Request.hpp"
#include "ConfigLocation.hpp"
#include "ConfigServer.hpp"
#include "GlobalServer.hpp"
#include "MIME.hpp"
#include <sys/socket.h>
#include <dirent.h>

#define TIMEOUT 5000

std::string createErrorResponse(ConfigServer& configServer, std::string errorCode);
vector<unsigned char> readRequestFile(const string& resource);
string getFileExtension(const string& filename);
string filetype(const string& url);
string getHandler(Request& req, ConfigServer& configServer, ConfigLocation& configLocation);
string deleteHandler(std::string uploadDirectory);
string otherHandler(ConfigServer& configServer);
string listdir(const string& path);
string postUploadHandler(Request& req, ConfigServer& configServer, ConfigLocation& configLocation);

#endif