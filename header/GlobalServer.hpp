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
# include <fcntl.h>
# include <unistd.h>
# include <string.h>
# include <errno.h>
# include <iostream>
# include "WebServerConfig.hpp"
# include <cstdlib>
# include <limits>
# include <cstring>


# define READ_BUFFER 1024

class GlobalServer
{
    private:

        // Structure to hold per-connection data.
        struct Connection 
        {
            int fd;              // Client socket file descriptor.
            std::string buffer;  // Buffer to accumulate received data.
        };

        // Global map to track connections by file descriptor.
        std::map<int, Connection*> connections;

        // Global epoll file descriptor.
        int epoll_fd;

        WebServerConfig webServerConfig;

        int setNonBlocking(int fd);
        int createAndBind(int port);

        //int stringToInt(const std::string& str);

        void removeConnection(Connection* conn);
        void addConnection(int client_fd) ;

    public:
        GlobalServer(WebServerConfig _webServerConfig);
        ~GlobalServer();
        GlobalServer(const GlobalServer& other);
        GlobalServer& operator=(const GlobalServer& other);

        void startServer();

        
        /*class InvalidStrToIntException: public std::exception
        {
            public:
                const char* what(void) const throw();
        };*/
};



#endif