/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utilConfig.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ssian <ssian@student.42singapore.sg>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/22 21:58:42 by ssian             #+#    #+#             */
/*   Updated: 2025/02/22 21:58:42 by ssian            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "../header/utilConfig.hpp"

std::string trim(const std::string& str)
{
    std::string::size_type start = str.find_first_not_of(" \t");
    if (start == std::string::npos)
        return ("");

    std::string::size_type end = str.find_last_not_of(" \t");
    return str.substr(start, end - start + 1);
}

void printMap(std::map<std::string, std::string> anyMap)
{
    std::map<std::string, std::string>::iterator it;
    for (it = anyMap.begin(); it != anyMap.end(); it++)
    {
        std::cout << it->first << " : " << it->second << std::endl; 
    }
}

void printVecStr(std::vector<std::string> vecStr)
{
    std::vector<std::string>::iterator it;
    for (it = vecStr.begin(); it < vecStr.end(); it++)
    {
        std::cout << *it << std::endl;
    } 
}

/* remove block[location] [/location]*/
std::string removeBlock(std::string configStr, std::string startMarker, std::string endMarker)
{
    size_t startPos = configStr.find(startMarker);
    while (startPos != std::string::npos) 
    {
        size_t endPos = configStr.find(endMarker, startPos);
        if (endPos == std::string::npos)                            // No matching end marker found.
            break;
        
        endPos += endMarker.length();                               // Move endPos to include the entire end marker.
        configStr.erase(startPos, endPos - startPos);               // Erase the block from the configuration.
        startPos = configStr.find(startMarker);                     // Look for the next "[location]" marker.
    }
    return (configStr);
}

std::map<std::string, std::string> extractKeyValuePair(std::string configStr)
{
    std::map<std::string, std::string> keyValueMap;
    std::istringstream iss(configStr);
    std::string line;
    
    while (std::getline(iss, line)) 
    {
        std::string trimmedLine = trim(line);
        if (trimmedLine.empty())                                // Skip empty lines.
            continue;  
        
        std::string::size_type pos = trimmedLine.find('=');     // Find the '=' separator.
        if (pos == std::string::npos)
            continue;                                           // Skip lines that don't contain '='.
        
        std::string key = trim(trimmedLine.substr(0, pos));     // Extract key and value substrings, and trim them.
        std::string value = trim(trimmedLine.substr(pos + 1));
        
        keyValueMap[key] = value;                               // Store the key-value pair in the map.
    }

    return (keyValueMap);
}

/**
    extract multiple between block lines in to a vector for further processing
    e.g multiple [location][/location]
*/
std::vector<std::string> extractBetweenBlockVec(std::string configStr, std::string startMarker, std::string endMarker)
{
    std::vector<std::string> betweenBlockVec;
    std::vector<std::string> blockLines;
    std::string line;
    bool inBlock = false;
    std::string blockStr;
    std::istringstream iss(configStr);                   //read str

    while (std::getline(iss, line)) 
    {
        std::string trimmed = trim(line);
        if (trimmed == startMarker)                      // "[location]"
        {
            inBlock = true;
            continue;                                   // Skip the marker line.
        }

        if (trimmed == endMarker)                       // "[/location]"
        {
            inBlock = false;
            std::vector<std::string>::iterator it;
            for (it = blockLines.begin(); it != blockLines.end(); it++)
            {
                blockStr = blockStr + '\n' + (*it);
            }
            betweenBlockVec.push_back(blockStr);         //add to vector each block lines in a str
            blockStr = "";                               //clear for next block use
            blockLines.clear(); 
        }

        if (inBlock) 
        {
            blockLines.push_back(trimmed);               //add line to same block vec
        }
    }

    return (betweenBlockVec);
}

std::string extractBlock(std::string configStr, std::string startMarker, std::string endMarker)
{
    std::istringstream iss(configStr);
    std::string line;
    bool inBlock = false;
    std::vector<std::string> blockLines;
    std::string blockStr;

    while (std::getline(iss, line)) 
    {
        std::string trimmed = trim(line);
        if (trimmed == startMarker)                       //"[global]"
        {
            inBlock = true;
            continue;                                    //check next line
        }
        if (trimmed == endMarker)                        //"[/global]"
        {
            inBlock = false;
            break;                                       // Stop reading, only look for 1 block
        }
        if (inBlock) 
        {
            blockLines.push_back(trimmed);
        }
    }

    for (std::vector<std::string>::iterator it = blockLines.begin(); it != blockLines.end(); it++)   //convert lines in vector to a string 
    {
        blockStr = blockStr + '\n' + (*it);
    }

    return (blockStr);
}


int parseContentLength(const std::string& request)
{
    std::string header = "Content-Length:";
    size_t pos = request.find(header);
    if (pos == std::string::npos)                                                   // Header field "Content-Length:" not found.
        return (0);                                                  
    
    pos += header.size();                                                           // Move position past "Content-Length:".
    
    while (pos < request.size() && (request[pos] == ' ' || request[pos] == '\t'))  // Skip any whitespace after the header.
    {
        pos++;
    }
    
    size_t end = request.find("\r\n", pos);                                         // Find the end of the line (CRLF).
    if (end == std::string::npos) 
    {
        end = request.size();
    }
    
    std::string valueStr = request.substr(pos, end - pos);                         // Extract the substring containing the numeric value.
    if (!isValidInt(valueStr))
        return (0);

    int contentLength = std::atoi(valueStr.c_str());                               // Convert the string to an integer.
    return contentLength;
}

std::string parseHeaderField(const std::string& request, std::string field)
{
    size_t pos = request.find(field);
    if (pos == std::string::npos)                                                   // Header not found.
    {
        return "";                                                 
    }
    
    pos += field.size();                                                           // Move position past field = "Content-Length:".
    
    
    while (pos < request.size() && (request[pos] == ' ' || request[pos] == '\t'))  // Skip any whitespace after the header.
    {
        pos++;
    }
    
    size_t end = request.find("\r\n", pos);                                         // Find the end of the line (CRLF).
    if (end == std::string::npos) 
    {
        end = request.size();
    }
    
    std::string valueStr = request.substr(pos, end - pos);                         // Extract the substring containing the numeric value.
    return (valueStr);
}

const char* InvalidStrToIntException::what(void) const throw()
{
    return ("Invalid string to int exception!");
}

int stringToInt(const std::string& str) 
{
    long lvalue;
    char* endptr;
    errno = 0;

    if (str == "")
        throw InvalidStrToIntException();

    lvalue = std::strtol(str.c_str(), &endptr, 10);

    if (*endptr != '\0' || errno == ERANGE)
        throw InvalidStrToIntException();

    if (lvalue >= std::numeric_limits<int>::min() && lvalue <= std::numeric_limits<int>::max())
        return (static_cast<int>(lvalue));
    else
        throw InvalidStrToIntException();
}

//valid port from 1024 - 65535
bool isValidPort(std::string portStr)
{
    int port;
    try 
    {
        port = stringToInt(portStr);
    }
    catch(const std::exception& e)
    {
        return (false);
    }

    if (port >= 1024 && port <= 65535)
        return (true);
    else
        return (false);
}

// int must >= 0 <=int_max
bool isValidInt(std::string valueStr)
{
    try 
    {
        if (stringToInt(valueStr) >= 0)
            return (true);
        else
            return (false);
    }
    catch(const std::exception& e)
    {
        return (false);
    }
}


std::string readFile(std::string configFile)
{
    std::string fileStr;
    std::string line;
    struct stat info;

    if (stat(configFile.c_str(), &info) != 0) 
    {
        std::cerr << "Error: Cannot access config file" << std::endl;
        exit(1);
    }
      
    if (S_ISDIR(info.st_mode))                                              // Check if it's a directory
    {
        std::cerr << "Error: config file is a directory" << std::endl;
        exit(1);
    }
    
    std::ifstream in(configFile.c_str());
    if (!in) 
    {
        std::cerr << "Error: Cannot open file!" << std::endl;
        exit(1);
    }

    while (std::getline(in, line)) 
    {
        fileStr = fileStr + '\n' + line;
    }
    in.close();

    return (fileStr);
}

/*split "Host: example.com:8080" by :*/
std::vector<std::string> splitHost(std::string hostStr)
{
    std::vector<std::string> hostVec;
    std::string::size_type start = 0;
    std::string::size_type pos;
    
    pos = hostStr.find(':', start);
    while (pos != std::string::npos)                                      // Loop until no more ':' characters are found
    {
        hostVec.push_back(trim(hostStr.substr(start, pos - start)));
        start = pos + 1;
        pos = hostStr.find(':', start);
    }
    hostVec.push_back(trim(hostStr.substr(start)));                        // Add the final token after the last ':'
    return (hostVec);
}

/* check hostStr contain : , host:port, e.g localhost:8080*/
bool isValidHostPort(std::string hostStr)
{
    std::string::size_type start = 0;
    std::string::size_type pos;

    pos = hostStr.find(':', start);
    if (pos == std::string::npos)                                           //no have ':'
    {
        return (false);
    }  
    return (true);
}

/*line = "hello.com localhost example.com" findStr="localhost"*/
bool isContainIn(std::string line, std::string findStr)
{
    std::istringstream iss(line);
    std::string token;

    while (iss >> token)                        // Split by whitespace and check each token.
    {
        if (token == findStr) 
        {
            return (true);
        }
    }
    return (false);
}


// Check if the URL begins with the requestPath and remove the requestPath portion from the URL and append the rest to the root.
std::string replacePath(const std::string& url, const std::string& requestPath, const std::string& root) 
{
    if (url.compare(0, requestPath.size(), requestPath) == 0) 
    {
        if (url.substr(requestPath.size()).find("/") == 0)                 //starts with "/"
        {
            return (root + url.substr(requestPath.size())); 
        }
        else
        {
            return (root + "/" + url.substr(requestPath.size()));          // add "/"
        }
    }
    return (url);                                          
}


std::string readServerFile(const std::string& filePath) 
{
    std::ifstream file(filePath.c_str(), std::ios::in | std::ios::binary);
    if (!file)
    {
        return "";
    }
    std::ostringstream contents;
    contents << file.rdbuf();
    return (contents.str());
}


/* get current time in milliseconds.*/
long getCurrentTimeMs() 
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (long)(tv.tv_sec * 1000 + tv.tv_usec / 1000);
}


std::string getDirectoryPath(const std::string& filePath) 
{
    std::size_t found = filePath.find_last_of("/"); 
    if (found != std::string::npos) 
    {
        return filePath.substr(0, found + 1);
    }
    return "";
}
