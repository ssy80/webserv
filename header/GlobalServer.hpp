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
# include <netinet/in.h>
# include <arpa/inet.h>
//# include <fcntl.h>
# include <unistd.h>
# include <string.h>
# include <errno.h>
# include <iostream>
# include "WebServerConfig.hpp"
# include <cstdlib>
# include <limits>
# include <cstring>
# include "AServer.hpp"

# include "Request.hpp"
# include "RequestParser.hpp"
# include "Response.hpp"
# include "ResHelper.hpp"
# include "MIME.hpp"

# include <algorithm>


# define READ_BUFFER 1024

class GlobalServer : public AServer
{
    private:
        struct Connection 
        {
            int fd;                    
            std::string buffer; 
            long lastActive;
            std::string responseBuffer;
            size_t bytesSent;
        };
        std::map<int, Connection*> connections;
        int epoll_fd;
        
        std::vector<int> listenFdsVec;          
        WebServerConfig webServerConfig;

        //int setNonBlocking(int fd);
        int createAndBind(int port);

        void removeConnection(Connection* conn);
        void addConnection(int client_fd) ;

        int getMaxBodySize(std::string requestStr);
        ConfigServer parseConfigServer(std::string requestStr);

        std::string handleRequest(std::string& requestStr);
        std::string getErrorResponse(std::string& requestStr, std::string errorCode);

        void createEpoll();
        void startListeningPort(std::vector<int> uniquePortsVec);
        void checkTimeoutConnections(ConfigGlobal& configGlobal);

        std::string upload_directory;
        
    public:
        GlobalServer(WebServerConfig _webServerConfig);
        ~GlobalServer();
        GlobalServer(const GlobalServer& other);
        GlobalServer& operator=(const GlobalServer& other);
        
        void startServer();

};



#endif