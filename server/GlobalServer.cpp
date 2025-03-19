/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GlobalServer.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daong <daong@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/23 21:08:17 by ssian             #+#    #+#             */
/*   Updated: 2025/03/13 15:13:32 by daong            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../header/GlobalServer.hpp"

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
/*int GlobalServer::setNonBlocking(int fd)
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
}*/

int GlobalServer::createAndBind(int port)
{
    int sockfd;
    //sockfd = socket(AF_INET, SOCK_STREAM, 0);    //int sockfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    sockfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (sockfd == -1) 
    {
        std::cerr << "Error: socket create" << std::endl; 
        throw exception();
    }

    int opt = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)    // Allow reuse of local addresses.
    {
        std::cerr << "Error: setsockopt" << std::endl; 
        throw exception();
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) == -1) 
    {
        std::cerr << "Error: bind listen port" << std::endl; 
        throw exception();
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
        throw exception();
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

       /* if (setNonBlocking(sockfd) < 0)                                                     //set socket to non blocking 
        {
            std::cerr << "Error: set non blocking" << std::endl; 
            exit(1);
        }*/

        if (listen(sockfd, SOMAXCONN) < 0)                                                  //set socket to listen
        {
            std::cerr << "Error: set listen port" << std::endl; 
            throw exception();
        }
        else
            this->listenFdsVec.push_back(sockfd);                                           //add to listen 

        struct epoll_event event;                                                           
        memset(&event, 0, sizeof(event));

        //event.events = EPOLLET | EPOLLIN;
        event.events = EPOLLIN;
        event.data.fd = sockfd;                                                             // For listening sockets, we can simply store the fd in epoll_event.data.fd.
        if (epoll_ctl(this->epoll_fd, EPOLL_CTL_ADD, sockfd, &event) < 0) 
        {
            std::cerr << "Error: epoll_ctl add listen socket" << std::endl; 
            throw exception();
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

/**check and removed timeout connection*/
/*void GlobalServer::checkTimeoutCGIProcess()
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
}*/


void GlobalServer::startServer()
{
    std::vector<ConfigServer> configServerVec = this->webServerConfig.getConfigServerVec(); //get number of [server]
    ConfigGlobal configGlobal = this->webServerConfig.getConfigGlobal();                    //get global [global]                                         
    std::vector<int> uniquePortsVec = this->webServerConfig.getUniquePortsVec();            //num of listening port depend on number of server   
    
    createEpoll();                                                                          
    startListeningPort(uniquePortsVec);

    this->upload_directory = configGlobal.getUploadDirectory();

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
            throw exception();
        }

  std::cerr << "connections: " << connections.size() << std::endl;

        int fd;
        int readFd;
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
                    struct sockaddr_in clientAddr;
                    socklen_t clientLen = sizeof(clientAddr);
                    int clientFd = accept(fd, (struct sockaddr *)&clientAddr, &clientLen);
                    //int clientFd = accept4(fd, (struct sockaddr *)&clientAddr, &clientLen, SOCK_NONBLOCK);
                    if (clientFd < 0)
                    {
                          break;
                    }
                    addConnection(clientFd);
            }
            else                                                                          //process event from a client fd
            {
                Connection* conn = static_cast<Connection*>(events[i].data.ptr);
                bool closeEvent = false;
                conn->lastActive = getCurrentTimeMs();

                if (ev & (EPOLLERR | EPOLLHUP))                                         //error or hang-up occurred on the CGI fd.
                {
                //std::cerr << "---1---- "  << std::endl;
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, conn->cgiOutFd, NULL);
                    close(conn->cgiOutFd);

                    int status;
                    pid_t result = waitpid(conn->cgiPid, &status, WNOHANG);
                    if (result <= 0)
                    {
                        kill(conn->cgiPid, SIGKILL);
                    }
                
                //std::cerr << " --getErrorResponse(conn->buffer --" << conn->buffer << std::endl;
                    conn->responseBuffer = getErrorResponse(conn->buffer, "500");
                    conn->bytesSent = 0;

                //std::cerr << " --(ev & (EPOLLERR | EPOLLHUP)) epoll_ctl --" << std::endl;

                    struct epoll_event event;
                    memset(&event, 0, sizeof(event));
                    event.data.ptr = conn;
                    event.events = EPOLLOUT;                                                //modify epoll_ctl for send.
                    if (epoll_ctl(this->epoll_fd, EPOLL_CTL_MOD, conn->fd, &event) < 0) 
                        std::cerr << "Error: epoll_ctl mod EPOLLOUT" << std::endl;
                }

                else if (ev & EPOLLIN)                                                         //reading
                { 
                //std::cerr << "---2---- " << std::endl;
                    readFd = conn->fd;
                    if (conn->cgiOutFd != -1)                                                  //cgi fd
                        readFd = conn->cgiOutFd;

                    ssize_t count;
                    while (true)                                                 
                    {
                        memset(buffer, 0, READ_BUFFER);
                        count = recv(readFd, buffer, sizeof(buffer), MSG_DONTWAIT);
                        if (count > 0)                                                              // data read
                        {
                            if (conn->cgiOutFd != -1)
                            {
                                conn->responseBuffer.append(buffer, count);
                //std::cerr << "--conn->responseBuffer--: " << conn->responseBuffer << std::endl;
                                checkForCGITimeout(conn);
                            }
                            else
                                conn->buffer.append(buffer, count);
                        } 
                        else if (count == 0)                                              // client closed connection
                        {
                            if (conn->cgiOutFd != -1)                                     // cgi EOF reached
                            {
                //std::cerr << "(count == 0) conn->responseBuffer: " << conn->responseBuffer << std::endl;
                                int status;
                                waitpid(conn->cgiPid, &status, 0);
                                if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)              // The CGI script failed.
                                {
                                    conn->responseBuffer = getErrorResponse(conn->buffer, "500");
                                    conn->bytesSent = 0;
                                }

                                epoll_ctl(epoll_fd, EPOLL_CTL_DEL, conn->cgiOutFd, NULL);
                                close(conn->cgiOutFd);

                //std::cerr << "--epoll_ctl mod EPOLLOUT--" << std::endl;
                                struct epoll_event event;
                                memset(&event, 0, sizeof(event));
                                event.data.ptr = conn;
                                event.events = EPOLLOUT;                                                //modify to wait for write event.
                                if (epoll_ctl(this->epoll_fd, EPOLL_CTL_MOD, conn->fd, &event) < 0) 
                                    std::cerr << "Error: epoll_ctl mod EPOLLOUT" << std::endl;
                            }
                            else
                            {
                                removeConnection(conn);
                                closeEvent = true;
                            }
                            break;
                        }
                        else if (count == -1)                                   // no more data, break out recv loop, wait for next read event
                        {
                            //if (errno == EAGAIN || errno == EWOULDBLOCK)      // no more data, stop recv
                            //    break; 
                            if (conn->cgiOutFd != -1)
                            {
                                //std::cerr << "(count == -1) cgi" << std::endl;
                                checkForCGITimeout(conn);
                            }
                            else
                                break;
                        }
                    }

                    if (closeEvent)
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
                            conn->responseBuffer = handleRequest(conn->buffer, conn);
                            conn->bytesSent = 0;
                            conn->buffer.clear();
                            if (conn->responseBuffer == "")                               //cgi process
                                break;
                        }
                    }
                    else
                    {
                        conn->responseBuffer = getErrorResponse(conn->buffer, "400");
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

                if (ev & EPOLLOUT )
                {
                //std::cerr << "---3---- " << std::endl;
                    int n;    
                    while (conn->bytesSent < conn->responseBuffer.size()) 
                    {
                        n = send(conn->fd, conn->responseBuffer.data() + conn->bytesSent, conn->responseBuffer.size() - conn->bytesSent, MSG_NOSIGNAL);            
                        if (n > 0) 
                        {
                            conn->bytesSent += n;
                            std::cerr << "--conn->responseBuffer.size()--: " << conn->responseBuffer.size() << std::endl;
                            std::cerr << "--bytes_sent--: " << conn->bytesSent << std::endl;
                //std::cerr << "--conn->responseBuffer--: " << conn->responseBuffer << std::endl;
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
        checkForCGITimeout();
    }
    
    for (size_t i = 0; i < this->listenFdsVec.size(); i++) 
    {
        close(this->listenFdsVec[i]);
    }
    close(this->epoll_fd);
}

void GlobalServer::checkForCGITimeout(Connection* conn)
{
    int status;
    pid_t result = waitpid(conn->cgiPid, &status, WNOHANG);
    if (result <= 0) //still active
    {
        if (getCurrentTimeMs() - conn->lastActive > TIMEOUT)
        {
            kill(conn->cgiPid, SIGKILL);
            std::cerr << "killed child due to time out" << std::endl;
            //std::cerr << "result waitpid: " << waitpid(conn->cgiPid, &status, 0) << std::endl;
        } 
    }  
} 

void GlobalServer::checkForCGITimeout() 
{
    int status;
    for (std::map<int, Connection*>::iterator it = this->connections.begin(); it != this->connections.end(); it++) 
    {
        Connection* conn = it->second;
        if (conn->cgiOutFd != -1)
        {
            pid_t result = waitpid(conn->cgiPid, &status, WNOHANG);
            if (result <= 0)
            {
                if (getCurrentTimeMs() - conn->lastActive > TIMEOUT)
                {
                    kill(conn->cgiPid, SIGKILL);
                    std::cerr << "killed child due to time out" << std::endl;
                    //std::cerr << "result waitpid: " << waitpid(conn->cgiPid, &status, 0) << std::endl;
                } 
            }  
        }
    }
}


/* Add a new client connection to epoll and store its context.*/
void GlobalServer::addConnection(int client_fd) 
{
    Connection* conn = new Connection;
    conn->fd = client_fd;
    conn->buffer = "";
    conn->lastActive = getCurrentTimeMs();
    conn->cgiOutFd = -1;
    
    connections[client_fd] = conn;

    struct epoll_event event;
    memset(&event, 0, sizeof(event));
    //event.events = EPOLLET | EPOLLIN;
    event.events = EPOLLIN;
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

    if (conn->cgiOutFd != -1)
    {
        epoll_ctl(this->epoll_fd, EPOLL_CTL_DEL, conn->cgiOutFd, NULL);
        /*if (epoll_ctl(this->epoll_fd, EPOLL_CTL_DEL, conn->cgiOutFd, NULL) < 0) 
        {
            std::cerr << "Error: epoll_ctl remove client cgi" << std::endl; 
        } */
        close(conn->cgiOutFd);
    }

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
        else if (errorCode == "500")
            statusCode = "500 Internal Server Error";    
    }

    if (filePath != "")
    {
        if (readServerFile(filePath) == "")                  //file not found
            filePath = "";
    }
                                    
    if (filePath == "")                                      //cannot find error page, send default error page
    {
        std::map<std::string, std::string> defaultErrorPageMap = configServer.getDefaultErrorPageMap();
        it = defaultErrorPageMap.find(errorCode);
        if (it != defaultErrorPageMap.end())
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
            else if (errorCode == "500")
                statusCode = "500 Internal Server Error";
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
    output = output + file + "\r\n";
    return (output);
}

std::string GlobalServer::handleRequest(std::string& requestStr, Connection* conn)
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

    std::cerr << "requestPath: " << requestPath << std::endl;
    std::cerr << "filePath: " << filePath << std::endl;

    std::string resp = "";

    if (req.version != "HTTP/1.1")                              //handle wrong version
    {
        return (createErrorResponse(configServer, "505"));
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

    //std::cout << "REQUEST STR: " << requestStr << std::endl;
    //std::cout << "REQ PRINT: " << std::endl;
    //req.print();
    if (req.method == "GET" && isContainIn(methods, "GET") && requestPath == "/cgi-bin" && !isDir(filePath))
    {
        std::cerr << "--cgi get request--" << std::endl;
        handleGetCGI(filePath, conn);
        return ("");
    }
    else if (req.method == "POST" && isContainIn(methods, "POST"))
    {
        if (req.url == "/cgi-bin/save_file.py"){
          std::cerr << "--cgi post request--" << std::endl;
          postCgiHandler(req, configServer, configLocation, this->upload_directory, conn);
          return ("");
        }
        if (req.url[req.url.length()-1] != '/' || req.headers["Content-Type"].find("multipart/form-data") == std::string::npos)
          return createErrorResponse(configServer, "422");
        return postUploadHandler(req, configServer, configLocation);
    }
    else if (req.method == "GET" && isContainIn(methods, "GET")){
        resp = getHandler(req, configServer, configLocation);
        resp.erase(resp.end()-1);
    }
    //else if (req.method == "POST" && isContainIn(methods, "POST"))
    //    resp = postHandler(req, configServer, configLocation, this->upload_directory);
    else if (req.method == "DELETE" && isContainIn(methods, "DELETE")){
      if (req.url != "/upload")
        return Response::ResBuilder()
          .sc(SC202)
          ->mc("Connection", "close")
          ->build()
          .toString();
      resp = deleteHandler(this->upload_directory);
    }
    else
        resp = otherHandler(configServer);
    resp += "\r\n";
  
    return resp;
}


void GlobalServer::handleGetCGI(std::string& filePath, Connection* conn)
{
    std::cerr << " -- handleGetCGI --" << std::endl;

    int cgiSp[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, cgiSp) < 0) 
    {
        perror("socketpair");
        return;
    }

    pid_t pid = fork();
    if (pid < 0) 
    {
        perror("fork");
        return;
    }

    if (pid == 0) 
    {   
        dup2(cgiSp[1], STDOUT_FILENO);
        close(cgiSp[1]);

        // Execute the CGI script.
        extern char **environ;  
        //std::string prog = "/bin/python3";
		//char* program =  const_cast<char*>(prog.c_str());
        //char* args[] = { program, const_cast<char*>("-u"), const_cast<char*>(filePath.c_str()), NULL };
    
        char* args[] = {const_cast<char*>(filePath.c_str()), NULL};
    // std::cerr << "args[0]: " << args[0] << std::endl;
    // std::cerr << "args[1]: " << args[1] << std::endl;
    // std::cerr << "args[2]: " << args[2] << std::endl;
		execve(args[0], args, environ);
        perror("execve");
        exit(1);
    }
    else  // Parent process
    {
        // Close the child's end of the pipes.
        close(cgiSp[1]);

        conn->cgiOutFd = cgiSp[0];
        conn->cgiPid = pid;

        // Add the CGI output fd to the epoll instance so that the main loop can read from it.
        struct epoll_event event;
        event.data.ptr = conn;         
        event.events = EPOLLIN | EPOLLERR | EPOLLHUP;                               //| EPOLLET; // Monitor for input (edge-triggered). EPOLLERR and EPOLLHUP
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, cgiSp[0], &event) < 0) 
            perror("epoll_ctl: add CGI fd socket");

        std::cerr << " -- parent end --" << std::endl;
    
    }
}


void GlobalServer::handlePostCGI(std::string& scriptPath, char* envp[], Connection* conn)  
{
    std::cerr << " -- handlePostCGI --" << std::endl;

    int cgiSp[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, cgiSp) < 0) 
    {
        perror("socketpair");
        return;
    }

    pid_t pid = fork();
    if (pid < 0) 
    {
        perror("fork");
        return;
    }

    if (pid == 0) 
    {   
        dup2(cgiSp[1], STDOUT_FILENO);
        close(cgiSp[1]);

        char* args[] = {const_cast<char*>(scriptPath.c_str()), NULL};
		execve(args[0], args, envp);
		std::cerr << "Exec failed: " << errno << std::endl;
		exit(1);
    }
    else  // Parent process
    {
        // Close the child's end of the pipes.
        close(cgiSp[1]);

        conn->cgiOutFd = cgiSp[0];
        conn->cgiPid = pid;

        // Add the CGI output fd to the epoll instance so that the main loop can read from it.
        struct epoll_event event;
        event.data.ptr = conn;         
        event.events = EPOLLIN | EPOLLERR | EPOLLHUP;                               //| EPOLLET; // Monitor for input (edge-triggered). EPOLLERR and EPOLLHUP
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, cgiSp[0], &event) < 0) 
            perror("epoll_ctl: add CGI fd socket");

        std::cerr << " -- parent end --" << std::endl;
    
    }
}

// post handler is used to upload 1 file through the cgi script
void GlobalServer::postCgiHandler(Request& req, ConfigServer& configServer, ConfigLocation& configLocation, std::string uploadDirectory, Connection* conn) {

    std::cerr << "-- start postCgiHandler: " << std::endl;

    (void) configServer;
	string PATH_INFO = replacePath(req.url, configLocation.getRequestPath(), configLocation.getRoot());
	
	if (req.headers["Content-Type"].find("multipart/form-data") != std::string::npos
		&& req.url.find("save_file.py") != std::string::npos
		&& !req.files.empty()
		&& !req.formFields.empty()) 
    {
    std::cerr << "-- multipart/form-data: " << std::endl;

		mkdir(uploadDirectory.c_str(), 0777);
		string upload_filename = (uploadDirectory + "/" + req.formFields["filename"]).substr(2);
		
		int idxEnd = req.files["filename"].size() - 3;
		string upload_content = req.files["filename"].substr(2, idxEnd);

		//if (upload_content.size() > 100) {
		//	return createErrorResponse(configServer, "413");
		//}
		
		std::vector<std::string> envVars;
		envVars.push_back("UPLOAD_FILENAME=" + upload_filename);
		envVars.push_back("UPLOAD_CONTENT=" + upload_content);
		
		std::vector<char*> envp;
		for (std::vector<std::string>::iterator it = envVars.begin(); it != envVars.end(); ++it) {
			envp.push_back(const_cast<char*>(it->c_str()));
		}
		envp.push_back(NULL);

    std::cerr << "-- 1 start handlePostCGI: " << std::endl;
		
		//vector<unsigned char> file = readRequestCGI(PATH_INFO, envp.data());
        handlePostCGI(PATH_INFO, envp.data(), conn);
        
		//if (file.empty()) {
		//	return createErrorResponse(configServer, "500");
		//}

		//string res;
		//res.insert(res.end(), file.begin(), file.end());
		//return res;
	}
	
	if (req.headers.find("Transfer-Encoding") != req.headers.end()
		&& req.headers["Transfer-Encoding"].find("chunked") != std::string::npos
		&& req.url.find("save_file.py") != std::string::npos
		&& !req.files.empty()) 
    {
    
    std::cerr << "-- Transfer-Encoding: " << std::endl;

		mkdir(uploadDirectory.c_str(), 0777);
		string upload_filename = (uploadDirectory + "/" + "chunks.txt").substr(2);
		//string upload_filename = req.formFields["filename"];
		
		// TEST STRING
		//string test = "4\r\nWiki\r\n7\r\npedia i\r\nB\r\nn \r\n chunkQ.\r\n0\r\n\r\n";
		//string upload_content = getChunks(test);
		
		// ACTUAL STRING
		string upload_content = getChunks(req.files["body"]);
		//if (upload_content.empty()) {
		//	return createErrorResponse(configServer, "400");
		//}
		
		std::vector<std::string> envVars;
		envVars.push_back("UPLOAD_FILENAME=" + upload_filename);
		envVars.push_back("UPLOAD_CONTENT=" + upload_content);

		std::vector<char*> envp;
		for (std::vector<std::string>::iterator it = envVars.begin(); it != envVars.end(); ++it) {
			envp.push_back(const_cast<char*>(it->c_str()));
		}
		envp.push_back(NULL);

    std::cerr << "-- 2 start handlePostCGI: " << std::endl;

        handlePostCGI(PATH_INFO, envp.data(), conn);
		//vector<unsigned char> file = readRequestCGI(PATH_INFO, envp.data());
		//if (file.empty()) {
		//	return createErrorResponse(configServer, "500");
		//}

		//string res;
		//res.insert(res.end(), file.begin(), file.end());
		//return res;
	}

	//else {
	//	return createErrorResponse(configServer, "405");
	//}

    std::cerr << "-- end handlePostCGI: " << std::endl;

}



std::string GlobalServer::getChunks(std::string& chunks) {
	std::cout << "GETCHUNKS: " << chunks << std::endl;
	size_t i = 0;
	string res;

	while (i < chunks.size()) {
	
		size_t clrfPos = chunks.find("\r\n", i);
		if (clrfPos == std::string::npos) {
			std::cerr << "Error: Invalid chunked encoding" << std::endl;
			break;
		}

		string chunkSizeHex = chunks.substr(i, clrfPos - i);
		std::stringstream ss;
		size_t chunkSize;
		ss << std::hex << chunkSizeHex;
		if (!(ss >> chunkSize)) {
			std::cerr << "Error: failed to parse chunk size: " << chunkSizeHex << std::endl;
		}

		i = clrfPos + 2;
		
		if (chunkSize == 0)
			break;
		
		res.append(chunks.substr(i, chunkSize));
		i += chunkSize + 2;

		std::cout << chunkSize << res << std::endl;
	}

    std::cerr << "RES: " << res << std::endl;
    return res;
}