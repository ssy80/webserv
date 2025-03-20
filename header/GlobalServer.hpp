/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GlobalServer.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ssian <ssian@student.42singapore.sg>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/23 21:07:56 by ssian             #+#    #+#             */
/*   Updated: 2025/02/23 21:07:57 by ssian            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GLOBAL_SERVER_HPP
# define GLOBAL_SERVER_HPP

# include <sys/epoll.h>
# include <sys/socket.h>
# include <sys/wait.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <unistd.h>
# include <string.h>
# include <errno.h>
# include <iostream>
# include <cstdlib>
# include <limits>
# include <cstring>
# include <algorithm>
# include <exception>

# include "AServer.hpp"
# include "WebServerConfig.hpp"
# include "Request.hpp"
# include "RequestParser.hpp"
# include "Response.hpp"
# include "ResHelper.hpp"
# include "MIME.hpp"

using std::exception;

# define READ_BUFFER 1024

struct Connection 
{
    int fd;                    
    std::string buffer; 
    long lastActive;
    std::string responseBuffer;
    size_t bytesSent;
    int cgiOutFd;
    pid_t cgiPid;
};

class GlobalServer : public AServer
{
    private:
        std::map<int, Connection*> connections;
        int epoll_fd;
        
        std::vector<int> listenFdsVec;          
        WebServerConfig webServerConfig;

        int createAndBind(int port);
        void removeConnection(Connection* conn);
        void addConnection(int client_fd) ;
        int getMaxBodySize(std::string requestStr);
        ConfigServer parseConfigServer(std::string requestStr);

        std::string handleRequest(std::string& requestStr, Connection* conn);
        std::string getErrorResponse(std::string& requestStr, std::string errorCode);

        void createEpoll();
        void startListeningPort(std::vector<int> uniquePortsVec);
        void checkTimeoutConnections(ConfigGlobal& configGlobal);

        void checkForCGITimeout();
        void checkForCGITimeout(Connection* conn);

        void handleGetCGI(std::string& filePath, Connection* conn);
        void handlePostCGI(std::string& filePath, char* envp[], Connection* conn);

        void postCgiHandler(Request& req, ConfigLocation& configLocation, std::string uploadDirectory, Connection* conn);
        std::string getChunks(std::string& chunks);

        void handleCgiError(Connection* conn);
        void handleCgiEof(Connection* conn);
        bool isValidServer(Connection* conn);
        bool isValidBodySize(Connection* conn);
        void setEpollout(Connection* conn);

        std::string upload_directory;
        
    public:
        GlobalServer(WebServerConfig _webServerConfig);
        ~GlobalServer();
        GlobalServer(const GlobalServer& other);
        GlobalServer& operator=(const GlobalServer& other);
        
        void startServer();

};



#endif