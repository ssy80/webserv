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
#include <iostream>

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


void GlobalServer::createEpoll()
{
    this->epoll_fd = epoll_create(10);                                                      //Create epoll instance.
    if (this->epoll_fd < 0) 
    {
        std::cerr << "Error: epoll_create" << std::endl; 
        exit(1);
    }
    std::cerr << "1this->epoll_fd" << this->epoll_fd << std::endl;
}

void GlobalServer::startListeningPort(std::vector<int> uniquePortsVec)
{
    int num_ports;
    int port;
    int sockfd;

    num_ports = uniquePortsVec.size();

    for (int i = 0; i < num_ports; i++)                                                      // Set up listening sockets for num of servers
    {
        port = uniquePortsVec[i];
        sockfd = createAndBind(port);                                                       //bind the port to listen

        if (setNonBlocking(sockfd) < 0)                                                     //set socket to non blocking 
        {
            std::cerr << "Error: set non blocking" << std::endl; 
            exit(1);
        }

        if (listen(sockfd, SOMAXCONN) < 0)                                                  //set socket to listen
        {
            std::cerr << "Error: set listen port" << std::endl; 
            exit(1);
        }
        else
            this->listenFdsVec.push_back(sockfd);                                                  //add to listen 

        struct epoll_event event;                                                           // For listening sockets, we can simply store the fd in epoll_event.data.fd.
        memset(&event, 0, sizeof(event));

        event.events = EPOLLET | EPOLLIN; //| EPOLLET;                                   // set level triggered(EPOLLIN) for listening sockets, edge trggered(EPOLLET) needed??
        event.data.fd = sockfd;
        if (epoll_ctl(this->epoll_fd, EPOLL_CTL_ADD, sockfd, &event) < 0) 
        {
            std::cerr << "Error: epoll_ctl add listen socket" << std::endl; 
            exit(1);
        }
        std::cout << "Listening on port: " << port << std::endl;
    }
}

/**check and removed timeout connection*/
void GlobalServer::checkTimeoutConnections(ConfigGlobal& configGlobal)
{
    long now = getCurrentTimeMs();
    std::vector<int> toRemove;
    for (std::map<int, Connection*>::iterator it = this->connections.begin(); it != this->connections.end(); it++) 
    {
        Connection* conn = it->second;
        if (now - conn->lastActive > configGlobal.getTimeout()) 
        {
            std::cout << "Connection timed out: " << conn->fd << std::endl;
            toRemove.push_back(conn->fd);
        }
    }

    int fdRemove;
    for (size_t i = 0; i < toRemove.size(); i++) 
    {
        fdRemove = toRemove[i];
        if (this->connections.find(fdRemove) != this->connections.end()) 
            removeConnection(this->connections[fdRemove]);
    }
}

void GlobalServer::startServer()
{
    std::vector<ConfigServer> configServerVec = this->webServerConfig.getConfigServerVec(); //get number of [server]
    ConfigGlobal configGlobal = this->webServerConfig.getConfigGlobal();                    //get global [global]                                         
    std::vector<int> uniquePortsVec = this->webServerConfig.getUniquePortsVec();            //num of listening port depend on number of server   
    
    createEpoll();                                                                          
    startListeningPort(uniquePortsVec);
    
    int max_events = configGlobal.getMaxEvents();
    struct epoll_event events[max_events];
    char buffer[READ_BUFFER];
    int nunEventFds;

    while (true)                                                                                   // check event loop.
    {                   
        nunEventFds = epoll_wait(this->epoll_fd, events, max_events, 1000);                 //get number events need to check
        if (nunEventFds < 0)
        {
            std::cerr << "Error: epoll_wait" << std::endl; 
            exit(1);
        }

    std::cerr << "connections: " << connections.size() << std::endl;

        int fd;
        bool isListen;
        for (int i = 0; i < nunEventFds; i++) 
        {
            uint32_t ev = events[i].events;
            fd = events[i].data.fd;                                               
            isListen = false;
            for (size_t i = 0; i < this->listenFdsVec.size(); i++) 
            {
                if (fd == this->listenFdsVec[i])                                // Check if the event is from a listening socket.
                {
                    isListen = true;
                    break;
                }
            }

            if (isListen)                                                      //event from listening socket, do accept new client connection 
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

                    //char client_ip[INET_ADDRSTRLEN];
                    //inet_ntop(AF_INET, &clientAddr.sin_addr, client_ip, sizeof(client_ip));
                    //std::cout << "Accepted connection from " <<  client_ip << ":" << ntohs(clientAddr.sin_port) << std::endl;
                }
            }  
            else                                              //process event from a client socket, recv data from client
            {
                Connection* conn = static_cast<Connection*>(events[i].data.ptr);
                bool closeConn = false;
                conn->lastActive = getCurrentTimeMs();

                if (ev & EPOLLIN)                               //reading
                { 
                    int count;
                    while (true)                                                 
                    {
                        memset(buffer, 0, READ_BUFFER);
                        count = recv(conn->fd, buffer, sizeof(buffer), 0);
                        if (count > 0) 
                        {
                            conn->buffer.append(buffer, count);
                        } 
                        else if (count == 0)                                    // client closed connection, close
                        {
                            closeConn = true;
                            removeConnection(conn);
                            break;
                        } 
                        else
                        {
                            //if (errno == EAGAIN || errno == EWOULDBLOCK)         // no more data, stop recv
                            //    break; 
                            //std::cerr << "Error: recv" << std::endl;            //other error when recv, close 
                            //closeConn = true;
                            break;
                        }
                    }

                    if (closeConn)
                        break;

                    size_t headerEnd = conn->buffer.find("\r\n\r\n");         //check got received complete header
                    if (headerEnd == std::string::npos)                       //header incomplete; wait for next EPOLLIN event.
                        break;

        ConfigServer configServer = parseConfigServer(conn->buffer);                        //get correct serve according to config
        //std::cout << "1max_body_size: " << configServer.getMaxBodySize() << std::endl;

                    int contentLength = parseContentLength(conn->buffer);    // Parse Content-Length from header
                    /*if (contentLength < 0) 
                    {
                        std::cerr << "Info: no Content-Length" << std::endl;
                            //        removeConnection(conn);
                            //        continue;
                    }
                    std::cout << "parse Content-Length: " <<  contentLength  << std::endl;*/

                    size_t total_expected = headerEnd + 4 + contentLength;              // Calculate expected total length: headers + CRLF CRLF + body.
                    bool isMaxBodySize = false;
                    if (configServer.getMaxBodySize() > 0 && (conn->buffer.size() - (headerEnd + 4) > (long unsigned int)configServer.getMaxBodySize()))
                    {
                        isMaxBodySize = true;
                    }
                    else
                    { 
                        if (conn->buffer.size() < total_expected)                           //Full request (headers + body) hasn't been received yet // The partial data remains in conn->buffer; wait for more data.
                            break;
                    }                                                      
                        
                    //process request here
                    std::cerr << "request_content: " << std::endl << conn->buffer << std::endl;      //come to here, means all recv

                    if (isMaxBodySize)
                    {
                        conn->responseBuffer = getErrorResponse(conn->buffer, "413");
                        conn->bytesSent = 0;
                        conn->buffer.clear();
                    }
                    else
                    {
                        conn->responseBuffer = handleRequest(conn->buffer);
                        conn->bytesSent = 0;
                        conn->buffer.clear();
                    std::cerr << "1conn->responseBuffer.size(): " << conn->responseBuffer.size() << std::endl;
                    }

                    struct epoll_event event;
                    memset(&event, 0, sizeof(event));
                    event.data.ptr = conn;
                    event.events = EPOLLOUT;                           //modify to wait for write event.
                    if (epoll_ctl(this->epoll_fd, EPOLL_CTL_MOD, conn->fd, &event) < 0) 
                    {
                        std::cerr << "Error: epoll_ctl mod EPOLLOUT" << std::endl;
                    }
                }
                else if (ev & EPOLLOUT && (conn->responseBuffer.empty())) 
                {
                    removeConnection(conn);
                }
                else if (ev & EPOLLOUT && (!conn->responseBuffer.empty()))            //sending part
                {
                    std::cerr << "conn->responseBuffer.size(): " << conn->responseBuffer.size() << std::endl;

                    int n;    
                    while (conn->bytesSent < conn->responseBuffer.size()) 
                    {
                        n = send(conn->fd, conn->responseBuffer.data() + conn->bytesSent, conn->responseBuffer.size() - conn->bytesSent, MSG_NOSIGNAL);            
                        if (n > 0) 
                        {
                            conn->bytesSent += n;
                            std::cerr << "--bytes_sent--: " << conn->bytesSent << std::endl;
                        } 
                        /*else if (n == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) 
                        
                            // Cannot send more now; break and wait for next EPOLLOUT.
                            break;
                        } */
                        else 
                        {
                            std::cerr << "Error: send" << std::endl;
                            //closeConn = true;
                            /*if (errno == EPIPE || errno == ECONNRESET) 
                            {
                                removeConnection(conn); 
                                break;
                            }*/
                            break;
                        }
                    }

                    if (conn->bytesSent == conn->responseBuffer.size())                   // If the entire response is sent, remove EPOLLOUT.
                    {
                        //conn->responseBuffer.clear(); 
                        //conn->bytesSent = 0;
                        
                        // Modify epoll event to remove EPOLLOUT.
                        /*struct epoll_event event;
                        memset(&event, 0, sizeof(event));
                        event.data.ptr = conn;
                        event.events = EPOLLET | EPOLLIN;
                        if (epoll_ctl(this->epoll_fd, EPOLL_CTL_MOD, conn->fd, &event) < 0) 
                        {
                            std::cerr << "Error: epoll_ctl mod to remove EPOLLOUT" << std::endl;
                            //closeConn = true;
                        }*/
                        removeConnection(conn);                // Close connection after response.
                    }
                }
            }   
        }
        checkTimeoutConnections(configGlobal);
    }
    
    for (size_t i = 0; i < this->listenFdsVec.size(); i++) 
    {
        close(this->listenFdsVec[i]);
    }
    close(this->epoll_fd);
}


// Add a new client connection to epoll and store its context.
void GlobalServer::addConnection(int client_fd) 
{
    Connection* conn = new Connection;
    conn->fd = client_fd;
    conn->buffer = "";
    conn->lastActive = getCurrentTimeMs();
    connections[client_fd] = conn;

    struct epoll_event event;
    memset(&event, 0, sizeof(event));
    event.events = EPOLLET | EPOLLIN;
    event.data.ptr = static_cast<void*>(conn);
    if (epoll_ctl(this->epoll_fd, EPOLL_CTL_ADD, client_fd, &event) < 0) 
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
    if (epoll_ctl(this->epoll_fd, EPOLL_CTL_DEL, conn->fd, NULL) < 0) 
    {
        std::cerr << "Error: epoll_ctl remove client" << std::endl; 
    }
    close(conn->fd);
    connections.erase(conn->fd);
    delete conn;
}

/*assign first server with the listen port to handle request if no matching servername found*/
ConfigServer GlobalServer::parseConfigServer(std::string requestStr)
{
    std::string hostStr = parseHeaderField(requestStr, "Host:");
    std::vector<std::string> hostVec = splitHost(hostStr);
    std::string server_name = hostVec[0];
    int listenPort = std::atoi(hostVec[1].c_str());
    
    std::vector<ConfigServer> configServerVec = this->webServerConfig.getConfigServerVec();
    ConfigServer configServer;

    std::vector<ConfigServer>::iterator it;
    for (it = configServerVec.begin(); it < configServerVec.end(); it++)                                
    {                                                  
        configServer = (*it);
        if (configServer.getListenPort() != listenPort)
        {
            configServerVec.erase(it);
        }
    }

    bool isServerNameMatch = false;
    for (it = configServerVec.begin(); it < configServerVec.end(); it++)                                
    {                                                  
        configServer = (*it);
        if (isContainIn(configServer.getServerName(), server_name))
        {
            isServerNameMatch = true;
            std::cout << "isServerNameMatch " << isServerNameMatch << std::endl;
            break;                           
        }
    }

    if (!isServerNameMatch)                      
        configServer = configServerVec[0];

    return (configServer);
}

bool compareConfigLocationDescending(const ConfigLocation& a, const ConfigLocation& b) 
{
    return a.getRequestPath().length() > b.getRequestPath().length();
}

std::string GlobalServer::getErrorResponse(std::string& requestStr, std::string errorCode)
{
    //match server
    ConfigServer configServer = parseConfigServer(requestStr);
    std::map<std::string, std::string> errorPageMap = configServer.getErrorPageMap();
    std::string filePath;
    std::map<std::string, std::string>::iterator it;

    it = errorPageMap.find(errorCode);
    if (it != errorPageMap.end())
    {
       filePath = it->second;
    }
    else
    {
        std::map<std::string, std::string> defaultErrorPageMap = configServer.getDefaultErrorPageMap();
        it = defaultErrorPageMap.find(errorCode);
        if (it != defaultErrorPageMap.end())
        {
            filePath = it->second;
        }
    }

    //read file
    std::string output; 
    std::string file = readServerFile(filePath);
    Response res = Response::ResBuilder()
									.sc(SC413)
                                    ->ct(MIME::KEY + MIME::HTML)
									->mc("Connection:", "close")
									->cl(file.size())
									->build();
    output = res.toString();
    output = output + file + '\0';
    return (output);
}

std::string GlobalServer::handleRequest(std::string& requestStr)
{
    //match location
    ConfigServer configServer = parseConfigServer(requestStr);
    std::vector<ConfigLocation> configLocationVec = configServer.getConfigLocationVec();

    // Sort the vector in descending order using the comparator function.
    std::sort(configLocationVec.begin(), configLocationVec.end(), compareConfigLocationDescending);

    Request req = RequestParser::parseRequest(requestStr);
    std::cout << "Method: " << req.method << std::endl;
    std::cout << "URL: " << req.url << std::endl;

    ConfigLocation configLocation;
    std::vector<ConfigLocation>::iterator it;
    for (it = configLocationVec.begin(); it < configLocationVec.end(); it++)
    {
        //std::cout << "request_path: " << (*it).getRequestPath() << std::endl;
        configLocation = (*it);
        std::string requestPath = configLocation.getRequestPath();

        if (req.url.rfind(requestPath, 0) == 0) // pos=0 limits the search to the prefix
        { 
            break;
        }
    }

    std::cout << std::endl;
    std::cout << "handleRequest request_path: " << configLocation.getRequestPath() << std::endl;
    std::cout << "handleRequest root: " << configLocation.getRoot() << std::endl;
    //std::cout << "getMaxBodySize max_body_size: " << configServer.getMaxBodySize() << std::endl;
    std::cout << std::endl;

    std::string requestPath = configLocation.getRequestPath();
    std::string root = configLocation.getRoot();
    std::string index = configLocation.getIndex();
    std::string autoindex = configLocation.getAutoIndex();
    std::string methods = configLocation.getMethods();
    std::string redirect = configLocation.getRedirect();

    std::string filePath = replacePath(req.url, requestPath, root);

std::cout << "filePath: " << filePath << std::endl;

std::string output;

    if (req.method == "GET" && isContainIn(methods, "GET"))
    {
        //read file
        std::string file = readServerFile(filePath);

        Response res = Response::ResBuilder()
									.sc(SC200)
									//->ct(MIME::KEY + MIME::HTML)
                                    ->ct(MIME::KEY + MIME::PNG)
									->mc("Connection:", "close")
									->cl(file.size())
									->build();
	    output = res.toString();

        output = output + file + '\0';

    }
    return (output);


    /*if (req.method == "GET" && isContainIn(methods, "GET"))
        return getHandler(req, configLocation);
    else if (req.method == "POST" && isContainIn(methods, "POST"))
        return postHandler(req, configLocation);
    else if (req.method == "DELETE" && isContainIn(methods, "DELETE"))
        return deleteHandler(req, configLocation);
    else
        return otherHandler();
}


