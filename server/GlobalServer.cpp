/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GlobalServer.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daong <daong@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/23 21:08:17 by ssian             #+#    #+#             */
/*   Updated: 2025/03/09 12:22:00 by daong            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/wait.h>

struct CGIProcess {
    int pipe_fd;
    int client_fd;
    std::vector<unsigned char> outputBuffer;
    pid_t pid;
};

std::map<int, CGIProcess> cgiProcesses;

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

/* set flags to non blocking makes I/O operations on the file descriptor non-blocking, 
   meaning functions like read() and write() will return immediately rather than 
   waiting for the operation to complete*/
int GlobalServer::setNonBlocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) 
    {
        std::cerr << "Error: fcntl F_GETFL" << std::endl;
        return (-1); //exit(1);
    }

    flags |= O_NONBLOCK;
    if (fcntl(fd, F_SETFL, flags) == -1) 
    {
        std::cerr << "Error: fcntl F_SETFL" << std::endl; 
        return (-1); //exit(1);
    }
    return (0);
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
        std::cerr << "Error: bind listen port" << std::endl; 
        exit(1);
    }
    return sockfd;
}

/*Create epoll instance*/
void GlobalServer::createEpoll()
{
    this->epoll_fd = epoll_create(10);                                                      
    if (this->epoll_fd < 0) 
    {
        std::cerr << "Error: epoll_create" << std::endl; 
        exit(1);
    }
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
            this->listenFdsVec.push_back(sockfd);                                           //add to listen 

        struct epoll_event event;                                                           
        memset(&event, 0, sizeof(event));

        event.events = EPOLLET | EPOLLIN;
        event.data.fd = sockfd;                                                             // For listening sockets, we can simply store the fd in epoll_event.data.fd.
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

    while (true)                                                                            // check event loop.
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

            if (isListen)                                                      //event from listening socket, accept new client connection 
            {
                while (true) 
                {
                    struct sockaddr_in clientAddr;
                    socklen_t clientLen = sizeof(clientAddr);
                    int clientFd = accept(fd, (struct sockaddr *)&clientAddr, &clientLen);
                    if (clientFd < 0)                                                       // No more connections.
                    {
                        break;
                    }
                    if (setNonBlocking(clientFd) < 0)
                    {
                        close(clientFd);
                        continue;
                    }
                    addConnection(clientFd);
                }
            }
            else                                                                          //process event from a client fd
            {
                Connection* conn = static_cast<Connection*>(events[i].data.ptr);
                bool closeConn = false;
                conn->lastActive = getCurrentTimeMs();

                if (ev & EPOLLIN)                                                         //reading
                { 
                    int count;
                    while (true)                                                 
                    {
                        memset(buffer, 0, READ_BUFFER);
                        count = recv(conn->fd, buffer, sizeof(buffer), 0);
                        if (count > 0)                                                    // data read
                        {
                            conn->buffer.append(buffer, count);
                        } 
                        else if (count == 0)                                              // client closed connection
                        {
                            closeConn = true;
                            removeConnection(conn);
                            break;
                        } 
                        else if (count == -1)                                   // no more data, break out recv loop, wait for next read event
                        {
                            //if (errno == EAGAIN || errno == EWOULDBLOCK)      // no more data, stop recv
                            //    break; 
                            break;
                        }
                    }

                    if (closeConn)
                        break;

                    size_t headerEnd = conn->buffer.find("\r\n\r\n");                     //check got received complete header
                    if (headerEnd == std::string::npos)                                   //header incomplete; wait for next EPOLLIN event.
                        break;

                    ConfigServer configServer = parseConfigServer(conn->buffer);         //get correct server according to config
                    bool isValidServer = false;
                    if (configServer.getListenPort() >= 1024 && configServer.getListenPort() <= 65535)
                        isValidServer = true;
                    if (isValidServer)
                    {
                        int contentLength = parseContentLength(conn->buffer);                // Parse Content-Length from header

                        size_t total_expected = headerEnd + 4 + contentLength;               // Calculate expected total length: headers + CRLF CRLF + body.
                        bool isMaxBodySize = false;
                        if(configServer.getMaxBodySize() > 0 && contentLength > configServer.getMaxBodySize())
                        {
                            isMaxBodySize = true;
                        }
                        else if (configServer.getMaxBodySize() > 0 && (conn->buffer.size() - (headerEnd + 4) > (long unsigned int)configServer.getMaxBodySize()))
                        {
                            isMaxBodySize = true;
                        }
                        else
                        { 
                            if (conn->buffer.size() < total_expected)                           //Full request (headers + body) not received yet, partial data remains in conn->buffer, wait for more data.
                                break;
                        }                                                  

                        if (isMaxBodySize)
                        {
                            conn->responseBuffer = getErrorResponse(conn->buffer, "413");
                            conn->bytesSent = 0;
                            conn->buffer.clear();
                        }
                        else
                        {
                            conn->responseBuffer = handleRequest(conn->buffer);
                            
                            if (conn->responseBuffer.empty())
                            {
                                conn->responseBuffer = getErrorResponse(conn->buffer, "404");
                            }

                            conn->bytesSent = 0;
                            conn->buffer.clear();
                        }
                    }
                    else
                    {
                        conn->responseBuffer = getErrorResponse(conn->buffer, "404");
                        conn->bytesSent = 0;
                        conn->buffer.clear();
                    }

                    struct epoll_event event;
                    memset(&event, 0, sizeof(event));
                    event.data.ptr = conn;
                    event.events = EPOLLOUT;                                                //modify to wait for write event.
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
                        else if(n == 0)                                                        //client close
                        {
                            removeConnection(conn);
                            break;
                        }
                        else if (n == -1)                                                    //Cannot send more now; break and wait for next EPOLLOUT.
                        {
                            break;
                        }
                        /*else if (n == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) 
                        
                            // Cannot send more now; break and wait for next EPOLLOUT.
                            break;
                        } */
                    }

                    if (conn->bytesSent == conn->responseBuffer.size())                   // If the entire response is sent, remove connection
                    {
                        removeConnection(conn);
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


/* Add a new client connection to epoll and store its context.*/
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

/* Remove a connection from epoll, close its socket.*/
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
    ConfigServer configServer;
    std::string hostStr = parseHeaderField(requestStr, "Host:");
    
    if (!isValidHostPort(hostStr))
        return (configServer);

    std::vector<std::string> hostVec = splitHost(hostStr);
    std::string server_name = hostVec[0];

    if (!isValidPort(hostVec[1].c_str()))
        return (configServer);

    int listenPort = std::atoi(hostVec[1].c_str());
    
    std::vector<ConfigServer> configServerVec = this->webServerConfig.getConfigServerVec();

    std::vector<ConfigServer>::iterator it;
    for (it = configServerVec.begin(); it < configServerVec.end(); )                                
    {                                                  
        if ((*it).getListenPort() != listenPort)
            it = configServerVec.erase(it);
        else
            it++;
    }

    bool isServerNameMatch = false;
    for (it = configServerVec.begin(); it < configServerVec.end(); it++)                                
    {                                                  
        if (isContainIn((*it).getServerName(), server_name))
        {
            isServerNameMatch = true;
            configServer = (*it);
            break;                           
        }
    }

    if (!isServerNameMatch && configServerVec.size() != 0)                      
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
    std::string filePath = "";
    std::map<std::string, std::string>::iterator it;
    std::string statusCode;

    it = errorPageMap.find(errorCode);                         //find error page
    if (it != errorPageMap.end())
    {
        filePath = it->second;
        if (errorCode == "501")
            statusCode = "501 Not Implemented";
        else if (errorCode == "413")
            statusCode = "413 Content Too Large";
        else if (errorCode == "404")
            statusCode = "404 Not Found";
        else if (errorCode == "400")
            statusCode = "400 Bad Request";
    }

    if (filePath != "")
    {
        if (readServerFile(filePath) == "")                  //file not found
            filePath = "";
    }
                                    
    if (filePath == "")                                      //cannot find error page, send default error page 404
    {
        std::map<std::string, std::string> defaultErrorPageMap = configServer.getDefaultErrorPageMap();
        it = defaultErrorPageMap.find("404");
        if (it != defaultErrorPageMap.end())
        {
            filePath = it->second;
            statusCode = "404 Not Found";
        }
    }

    //read file
    std::string output; 
    std::string file = readServerFile(filePath);
    Response res = Response::ResBuilder()
									.sc(statusCode)
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
    //match server
    ConfigServer configServer = parseConfigServer(requestStr);
    std::vector<ConfigLocation> configLocationVec = configServer.getConfigLocationVec();

    std::sort(configLocationVec.begin(), configLocationVec.end(), compareConfigLocationDescending);                  // Sort the vector in descending order using the comparator function.

    Request req = RequestParser::parseRequest(requestStr);
    
    std::string urlDirPath = getDirectoryPath(req.url);

    ConfigLocation configLocation;
    std::vector<ConfigLocation>::iterator it;
    for (it = configLocationVec.begin(); it < configLocationVec.end(); it++)                                        //get [location] params matching the request_path
    {
        configLocation = (*it);
        std::string requestPath = configLocation.getRequestPath();

        if (urlDirPath.rfind(requestPath, 0) == 0)
            break;
    }

    std::string requestPath = configLocation.getRequestPath();
    std::string root = configLocation.getRoot();
    std::string index = configLocation.getIndex();
    std::string autoindex = configLocation.getAutoIndex();
    std::string methods = configLocation.getMethods();
    std::string redirect = configLocation.getRedirect();
    std::string filePath = replacePath(req.url, requestPath, root);            //map request_path to root in [location]

    std::string resp;

    if (req.version != "HTTP/1.1")                              //handle wrong version
    {
        return (getErrorResponse(requestStr, "501"));
    }

    if (!redirect.empty())                                     //handle redirect
    {
        Response res = Response::ResBuilder()
                .sc(SC301)
                ->mc("Location", redirect)
                ->mc("Connection", "close")
                ->build();
        resp = res.toString();
        return (resp);
    }

    std::cout << "REQUEST STR: " << requestStr << std::endl;
    std::cout << "REQ PRINT: " << std::endl;
    req.print();

    std::cout << "UPLOADD: " << this->upload_directory << std::endl;
    
    if (req.method == "GET" && isContainIn(methods, "GET"))
        resp = getHandler(req, configServer, configLocation);
    else if (req.method == "POST" && isContainIn(methods, "POST"))
        resp = postHandler(req, configServer, configLocation, this->upload_directory);
    else if (req.method == "DELETE" && isContainIn(methods, "DELETE"))
        resp = deleteHandler(this->upload_directory);
    else
        resp = otherHandler(configServer);
    resp[resp.size()-1] = '\0';
  
    return resp;
}


