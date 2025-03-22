/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utilConfig.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daong <daong@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/22 21:58:42 by ssian             #+#    #+#             */
/*   Updated: 2025/03/22 13:53:02 by daong            ###   ########.fr       */
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

std::string removeBlock(std::string configStr, std::string startMarker, std::string endMarker)
{
    size_t startPos = configStr.find(startMarker);
    while (startPos != std::string::npos) 
    {
        size_t endPos = configStr.find(endMarker, startPos);
        if (endPos == std::string::npos)
            break;
        
        endPos += endMarker.length();
        configStr.erase(startPos, endPos - startPos);
        startPos = configStr.find(startMarker);
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
        if (trimmedLine.empty())
            continue;  
        
        std::string::size_type pos = trimmedLine.find('=');
        if (pos == std::string::npos)
            continue;
        
        std::string key = trim(trimmedLine.substr(0, pos));
        std::string value = trim(trimmedLine.substr(pos + 1));
        
        keyValueMap[key] = value;
    }

    return (keyValueMap);
}

std::vector<std::string> extractBetweenBlockVec(std::string configStr, std::string startMarker, std::string endMarker)
{
    std::vector<std::string> betweenBlockVec;
    std::vector<std::string> blockLines;
    std::string line;
    bool inBlock = false;
    std::string blockStr;
    std::istringstream iss(configStr);

    while (std::getline(iss, line)) 
    {
        std::string trimmed = trim(line);
        if (trimmed == startMarker)
        {
            inBlock = true;
            continue;
        }

        if (trimmed == endMarker)
        {
            inBlock = false;
            std::vector<std::string>::iterator it;
            for (it = blockLines.begin(); it != blockLines.end(); it++)
            {
                blockStr = blockStr + '\n' + (*it);
            }
            betweenBlockVec.push_back(blockStr);
            blockStr = "";
            blockLines.clear(); 
        }

        if (inBlock) 
        {
            blockLines.push_back(trimmed);
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
        if (trimmed == startMarker)
        {
            inBlock = true;
            continue;
        }
        if (trimmed == endMarker)
        {
            inBlock = false;
            break;
        }
        if (inBlock) 
        {
            blockLines.push_back(trimmed);
        }
    }

    for (std::vector<std::string>::iterator it = blockLines.begin(); it != blockLines.end(); it++)
    {
        blockStr = blockStr + '\n' + (*it);
    }

    return (blockStr);
}


int parseContentLength(const std::string& request)
{
    std::string header = "Content-Length:";
    size_t pos = request.find(header);
    if (pos == std::string::npos)
        return (0);                                                  
    
    pos += header.size();
    
    while (pos < request.size() && (request[pos] == ' ' || request[pos] == '\t'))
    {
        pos++;
    }
    
    size_t end = request.find("\r\n", pos);
    if (end == std::string::npos) 
    {
        end = request.size();
    }
    
    std::string valueStr = request.substr(pos, end - pos);
    if (!isValidInt(valueStr))
        return (0);

    int contentLength = std::atoi(valueStr.c_str());
    return contentLength;
}

std::string parseHeaderField(const std::string& request, std::string field)
{
    size_t pos = request.find(field);
    if (pos == std::string::npos)
    {
        return "";                                                 
    }
    
    pos += field.size();
    
    
    while (pos < request.size() && (request[pos] == ' ' || request[pos] == '\t'))
    {
        pos++;
    }
    
    size_t end = request.find("\r\n", pos);
    if (end == std::string::npos) 
    {
        end = request.size();
    }
    
    std::string valueStr = request.substr(pos, end - pos);
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
        throw exception();
    }
      
    if (S_ISDIR(info.st_mode))
    {
        std::cerr << "Error: config file is a directory" << std::endl;
        throw exception();
    }
    
    std::ifstream in(configFile.c_str());
    if (!in) 
    {
        std::cerr << "Error: Cannot open file!" << std::endl;
        throw exception();
    }

    while (std::getline(in, line)) 
    {
        fileStr = fileStr + '\n' + line;
    }
    in.close();

    return (fileStr);
}

bool isDir(std::string filepath)
{
    struct stat info;

    if (stat(filepath.c_str(), &info) != 0) 
    {
        return (false);
    }
      
    if (S_ISDIR(info.st_mode))
    {
        return (true);
    }
    
    return (false);
}

std::vector<std::string> splitHost(std::string hostStr)
{
    std::vector<std::string> hostVec;
    std::string::size_type start = 0;
    std::string::size_type pos;
    
    pos = hostStr.find(':', start);
    while (pos != std::string::npos)
    {
        hostVec.push_back(trim(hostStr.substr(start, pos - start)));
        start = pos + 1;
        pos = hostStr.find(':', start);
    }
    hostVec.push_back(trim(hostStr.substr(start)));
    return (hostVec);
}

bool isValidHostPort(std::string hostStr)
{
    std::string::size_type start = 0;
    std::string::size_type pos;

    pos = hostStr.find(':', start);
    if (pos == std::string::npos)
    {
        return (false);
    }  
    return (true);
}

bool isContainIn(std::string line, std::string findStr)
{
    std::istringstream iss(line);
    std::string token;

    while (iss >> token)
    {
        if (token == findStr) 
        {
            return (true);
        }
    }
    return (false);
}

std::string replacePath(const std::string& url, const std::string& requestPath, const std::string& root) 
{
    if (url.compare(0, requestPath.size(), requestPath) == 0) 
    {
        if (url.substr(requestPath.size()).find("/") == 0)
        {
            return (root + url.substr(requestPath.size())); 
        }
        else
        {
            return (root + "/" + url.substr(requestPath.size()));
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
