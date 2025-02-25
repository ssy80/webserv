/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServerConfig.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ssian <ssian@student.42singapore.sg>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/22 19:53:45 by ssian             #+#    #+#             */
/*   Updated: 2025/02/22 19:53:46 by ssian            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "../header/WebServerConfig.hpp"

WebServerConfig::WebServerConfig(){}

WebServerConfig::~WebServerConfig(){}

WebServerConfig::WebServerConfig(const WebServerConfig& other)
{
    this->configGlobal = other.configGlobal;
    this->configServerVec = other.configServerVec;
}

WebServerConfig& WebServerConfig::operator=(const WebServerConfig& other)
{
    if (this == &other)
        return (*this);
    
    this->configGlobal = other.configGlobal;
    this->configServerVec = other.configServerVec;

    return (*this);
}

void WebServerConfig::parseWebServerConfigFile(std::string configFile)
{
    std::string configFileStr;
    configFileStr = readFile(configFile);                                          //read config file into string

    std::string globalStr = extractBlock(configFileStr, "[global]", "[/global]");
    this->configGlobal.parseConfigGlobal(globalStr);                               //parse the globalStr to key/value in ConfigGlobal

    std::vector<std::string> configServerStrVec = extractBetweenBlockVec(configFileStr, "[server]", "[/server]");

    std::vector<std::string>::iterator it;
    for (it = configServerStrVec.begin(); it < configServerStrVec.end(); it++)    
    {
        ConfigServer configServer;
        configServer.parseConfigServer(*it);                                      // parse each server string to ConfigServer obj 
        this->configServerVec.push_back(configServer);                            
    }

}

/*std::vector<std::string> WebServerConfig::getServerStr(std::string configFileStr)
{
    std::vector<std::string> configServerStrVec;

    std::istringstream iss(configFileStr);
    std::string line;
    bool inServer = false;
    std::vector<std::string> serverLines;
    std::string serverStr;

    while (std::getline(iss, line)) 
    {
        std::string trimmed = trim(line);
        if (trimmed == "[server]") 
        {
            inServer = true;
            continue; // Skip the marker line.
        }
        if (trimmed == "[/server]") 
        {
            inServer = false;
            //break;    // Stop reading after the global block.
            for (std::vector<std::string>::iterator it = serverLines.begin(); it != serverLines.end(); it++)
            {
                serverStr = serverStr + '\n' + (*it);
            }
            configServerStrVec.push_back(serverStr);
            serverStr = "";
            serverLines.clear();
        }
        if (inServer) 
        {
            serverLines.push_back(trimmed);
        }
    }

    return (configServerStrVec);
}*/





/*std::map<std::string, std::string> WebServerConfig::parseGlobalStr(std::string globalStr)
{
    std::map<std::string, std::string> configGlobalMap;
    std::istringstream iss(globalStr);
    std::string line;
    
    while (std::getline(iss, line)) 
    {
        // Trim the line to remove extra whitespace.
        std::string trimmedLine = trim(line);
        if (trimmedLine.empty())
            continue;  // Skip empty lines.
        
        // Find the '=' separator.
        std::string::size_type pos = trimmedLine.find('=');
        if (pos == std::string::npos)
            continue;  // Skip lines that don't contain '='.
        
        // Extract key and value substrings, and trim them.
        std::string key = trim(trimmedLine.substr(0, pos));
        std::string value = trim(trimmedLine.substr(pos + 1));
        
        // Store the key-value pair in the map.
        configGlobalMap[key] = value;
    }
    return configGlobalMap;
}*/

/*std::string WebServerConfig::getGlobalStr(std::string configStr)
{
    // Use an istringstream to read the configuration string line by line.
    std::istringstream iss(configStr);
    std::string line;
    bool inGlobal = false;
    std::vector<std::string> globalLines;
    std::string globalStr;

    while (std::getline(iss, line)) 
    {
        std::string trimmed = trim(line);
        if (trimmed == "[global]") 
        {
            inGlobal = true;
            continue; // Skip the marker line.
        }
        if (trimmed == "[/global]") 
        {
            inGlobal = false;
            break;    // Stop reading after the global block.
        }
        if (inGlobal) 
        {
            globalLines.push_back(trimmed);
        }
    }

    // Output the lines found within the [global] block.
    //std::cout << "Lines in the [global] block:" << std::endl;
    for (std::vector<std::string>::iterator it = globalLines.begin(); it != globalLines.end(); it++) 
    {
        //std::cout << *it << std::endl;
        globalStr = globalStr + '\n' + (*it);
    }

    return (globalStr);

}*/

std::string WebServerConfig::readFile(std::string configFile)
{
    std::string fileStr;
    std::string line;
    
    std::ifstream in(configFile.c_str());
    if (!in) 
    {
        std::cerr << "Error: Cannot open config file!" << std::endl;
        exit(1);
    }

    while (std::getline(in, line)) 
    {
        fileStr = fileStr + '\n' + line;
    }
    in.close();

    return (fileStr);
}


ConfigGlobal WebServerConfig::getConfigGlobal()
{
    return (this->configGlobal);
}

std::vector<ConfigServer> WebServerConfig::getConfigServerVec()
{
    return (this->configServerVec);
}
