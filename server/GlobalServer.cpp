/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GlobalServer.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ssian <ssian@student.42singapore.sg>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/23 21:08:17 by ssian             #+#    #+#             */
/*   Updated: 2025/02/23 21:08:18 by ssian            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "../header/GlobalServer.hpp"

GlobalServer::GlobalServer(WebServerConfig _webServerConfig): webServerConfig(_webServerConfig){}

GlobalServer::~GlobalServer(){}

GlobalServer::GlobalServer(const GlobalServer& other): webServerConfig(other.webServerConfig)
{}

GlobalServer& GlobalServer::operator=(const GlobalServer& other)
{
    if (this == &other)
        return (*this);
    this->webServerConfig = other.webServerConfig;
    return (*this);
}


int GlobalServer::setNonBlocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) 
    {
        std::cerr << "Error: fcntl F_GETFL" << std::endl;
        exit(1);
    }

    flags |= O_NONBLOCK;                                    //set flags to non blocking makes I/O operations on the file descriptor non-blocking, meaning functions like read() and write() will return immediately rather than waiting for the operation to complet
    if (fcntl(fd, F_SETFL, flags) == -1) 
    {
        std::cerr << "Error: fcntl F_SETFL" << std::endl; 
        exit(1);
    }
    return 0;
}

int GlobalServer::createAndBind(int port)
{
    int sockfd;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) 
    {
        std::cerr << "Error: socket create" << std::endl; 
        exit(1);
    }

    int opt = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)    // Allow reuse of local addresses.
    {
        std::cerr << "Error: setsockopt" << std::endl; 
        exit(1);
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) == -1) 
    {
        std::cerr << "Error: bind" << std::endl; 
        exit(1);
    }

    return sockfd;
}

void GlobalServer::startServer()
{
    std::vector<ConfigServer> configServerVec = this->webServerConfig.getConfigServerVec(); //get number of [server]
    ConfigGlobal configGlobal = this->webServerConfig.getConfigGlobal();                    //get global [global]

    int num_ports = configServerVec.size();                                                 //num of listening port depend on number of server                                                           //vector of ports to listen
    std::vector<int> listenFdsVec;
    
    this->epoll_fd = epoll_create(10);                                                      //Create epoll instance.
    if (this->epoll_fd < 0) 
    {
        std::cerr << "Error: epoll_create" << std::endl; 
        exit(1);
    }

    int port;
    int sockfd;
    for (int i = 0; i < num_ports; i++)                                                      // Set up listening sockets for num of servers
    {
        //port  = stringToInt((configServerVec[i].getKeyValueMap())["listen"]);               //get [server] listening=8080
        port  = std::atoi((configServerVec[i].getKeyValueMap())["listen"].c_str());               //get [server] listening=8080
        
        sockfd = createAndBind(port);                                                       //bind the port to listen

        if (setNonBlocking(sockfd) < 0)                                                     //set socket to non blocking 
        {
            std::cerr << "Error: set non blocking" << std::endl; 
            exit(1);
        }

        if (listen(sockfd, SOMAXCONN) < 0)                                                  //set socket to listen
        {
            std::cerr << "Error: set listen" << std::endl; 
            exit(1);
        }
        else
            listenFdsVec.push_back(sockfd);                                                  //add to listen 

        struct epoll_event event;                                                           // For listening sockets, we can simply store the fd in epoll_event.data.fd.
        memset(&event, 0, sizeof(event));

        event.events = EPOLLIN;                                   // set level triggered(EPOLLIN) for listening sockets, edge trggered(EPOLLET) needed??
        event.data.fd = sockfd;
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sockfd, &event) < 0) 
        {
            std::cerr << "Error: epoll_ctl add listen socket" << std::endl; 
            exit(1);
        }
        std::cout << "Listening on port: " << port << std::endl;
    }

    //int max_events = stringToInt((configGlobal.getKeyValueMap())["max_events"]);
    int max_events = std::atoi((configGlobal.getKeyValueMap())["max_events"].c_str());
    
    struct epoll_event events[max_events];
    char buffer[READ_BUFFER];

    int nunEventFds;
    while (true)                                                                           // check event loop.
    {
        nunEventFds = epoll_wait(epoll_fd, events, max_events, -1);                        //get number events need to recv
        if (nunEventFds < 0)
        {
            std::cerr << "Error: epoll_wait" << std::endl; 
            exit(1);
        }

        int fd;
        bool isListen;
        for (int i = 0; i < nunEventFds; i++) 
        {
            fd = events[i].data.fd;                                               
            isListen = false;
            for (size_t i = 0; i < listenFdsVec.size(); i++) 
            {
                if (fd == listenFdsVec[i])                                // Check if the event is from a listening socket.
                {
                    isListen = true;
                    break;
                }
            }

            if (isListen)                               //event from listening socket, do accept new client connection 
            {
                while (true) 
                {
                    struct sockaddr_in clientAddr;
                    socklen_t clientLen = sizeof(clientAddr);
                    int clientFd = accept(fd, (struct sockaddr *)&clientAddr, &clientLen);
                    if (clientFd < 0) 
                    {
                        if (errno == EAGAIN || errno == EWOULDBLOCK)               // No more connections.
                            break;
                        std::cerr << "Error: accept connection" << std::endl; 
                        break;
                    }
                    if (setNonBlocking(clientFd) < 0) 
                    {
                        close(clientFd);
                        continue;
                    }
                    addConnection(clientFd);

                    char client_ip[INET_ADDRSTRLEN];
                    inet_ntop(AF_INET, &clientAddr.sin_addr, client_ip, sizeof(client_ip));
                    std::cout << "Accepted connection from " <<  client_ip << ":" << ntohs(clientAddr.sin_port) << std::endl;
                }
            }  
            else                                              //process event from a client socket, recv data from client
            {
                
                Connection* conn = static_cast<Connection*>(events[i].data.ptr);
                bool closeConn = false;

                int count;
                while (true)                                                 
                {
                    count = recv(conn->fd, buffer, sizeof(buffer), 0);
                    if (count > 0) 
                    {
                        conn->buffer.append(buffer, count);
                    } 
                    else if (count == 0)                                    // client closed connection, close
                    {
                        closeConn = true;
                        break;
                    } 
                    else 
                    {
                        if (errno == EAGAIN || errno == EWOULDBLOCK)         // no more data, stop recv
                            break; 
                        
                        std::cerr << "Error: recv" << std::endl;            //other error when recv, close 
                        closeConn = true;
                        break;
                    }
                }

                size_t headerEnd = conn->buffer.find("\r\n\r\n");         //check got received complete header
                if (headerEnd == std::string::npos)                       //header incomplete; wait for next EPOLLIN event.
                {
                    if (closeConn)
                        removeConnection(conn);
                    continue;
                }

                
                int contentLength = parseContentLength(conn->buffer);    // Parse Content-Length from header
                if (contentLength < 0) 
                {
                    std::cerr << "Info: no Content-Length" << std::endl;
                        //        removeConnection(conn);
                        //        continue;
                }

                std::cout << "parse Content-Length: " <<  contentLength  << std::endl;

                size_t total_expected = headerEnd + 4 + contentLength;              // Calculate expected total length: headers + CRLF CRLF + body.
                if (conn->buffer.size() < total_expected)                           //Full request (headers + body) hasn't been received yet
                    continue;                                                       // The partial data remains in conn->buffer; wait for more data.
                    
            //process request here
                std::cout << "request_content: " << std::endl << conn->buffer << std::endl;      //come to here, means all recv
                // At this point, we assume the request is complete.
                // (For a real server, you'd parse headers, Content-Length, etc.)
                // Send a fixed HTTP response.


            //send response
                const char* response =
                    "HTTP/1.1 200 OK\r\n"
                    "Content-Length: 11\r\n"
                    "Content-Type: text/plain\r\n"
                    "\r\n"
                    "Hello World";

                size_t response_len = strlen(response);
                size_t bytes_sent = 0;
                while (bytes_sent < response_len) 
                {
                    int n = send(conn->fd, response + bytes_sent, response_len - bytes_sent, 0);
                    if (n > 0) 
                    {
                        bytes_sent += n;
                    } 
                    else if (n == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) 
                    {
                        //??? Would block; in production, you'd wait for EPOLLOUT.
                        continue;
                    } 
                    else 
                    {
                        std::cerr << "Error: send" << std::endl;
                        closeConn = true;
                        break;
                    }
                }
                removeConnection(conn);                // Close connection after response.
            }
        }
    }

    // Cleanup (never reached in this infinite loop)
    for (size_t i = 0; i < listenFdsVec.size(); i++) 
    {
        close(listenFdsVec[i]);
    }
    close(epoll_fd);
}

/*int GlobalServer::stringToInt(const std::string& str) 
{
    long lvalue;
    char* endptr;
    errno = 0;

    if (str == "")
        throw GlobalServer::InvalidStrToIntException();

    lvalue = std::strtol(str.c_str(), &endptr, 10);

    if (*endptr != '\0' || errno == ERANGE)
        throw GlobalServer::InvalidStrToIntException();

    if (lvalue >= std::numeric_limits<int>::min() && lvalue <= std::numeric_limits<int>::max())
        return (static_cast<int>(lvalue));
    else
        throw GlobalServer::InvalidStrToIntException();
}*/

/*const char* GlobalServer::InvalidStrToIntException::what(void) const throw()
{
    return ("Invalid string to int exception!");
}*/

// Add a new client connection to epoll and store its context.
void GlobalServer::addConnection(int client_fd) 
{
    Connection* conn = new Connection;
    conn->fd = client_fd;
    conn->buffer = "";
    connections[client_fd] = conn;

    struct epoll_event event;
    memset(&event, 0, sizeof(event));
    event.events = EPOLLIN; // | EPOLLET; // Edge-triggered read events.
    event.data.ptr = static_cast<void*>(conn);
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event) < 0) 
    {
        std::cerr << "Error: epoll_ctl add client" << std::endl; 
        close(client_fd);
        delete conn;
        connections.erase(client_fd);
    }
}

// Remove a connection from epoll, close its socket, and free its context.
void GlobalServer::removeConnection(Connection* conn)
{
    if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, conn->fd, NULL) < 0) 
    {
        std::cerr << "Error: epoll_ctl remove client" << std::endl; 
    }
    close(conn->fd);
    connections.erase(conn->fd);
    delete conn;
}