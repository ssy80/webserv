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
    if (configServerStrVec.size() == 0)
    {
        std::cerr << "Error: invalid config [server]" << std::endl;
        exit(1);
    }

    std::vector<std::string>::iterator it;
    for (it = configServerStrVec.begin(); it < configServerStrVec.end(); it++)    
    {
        ConfigServer configServer;
        configServer.parseConfigServer(*it);                                      // parse each server string to ConfigServer obj 
        this->configServerVec.push_back(configServer);                            
    }

    if (!validate())
    {
        std::cerr << "Error: invalid config [server]" << std::endl;
        exit(1);
    }
}

/*check only unique listening port for each servers*/
bool WebServerConfig::validate()
{
    std::vector<int> listenPortsVec;
    std::vector<ConfigServer>::iterator it;
    for (it = this->configServerVec.begin(); it < this->configServerVec.end(); it++)
    {
       for (size_t i = 0; i < listenPortsVec.size(); i++)
        {
            if (listenPortsVec[i] == (*it).getListenPort())
            {
                return (false);
            }
        }
        listenPortsVec.push_back((*it).getListenPort());
    }
    return (true);    
}

ConfigGlobal WebServerConfig::getConfigGlobal()
{
    return (this->configGlobal);
}

std::vector<ConfigServer> WebServerConfig::getConfigServerVec()
{
    return (this->configServerVec);
}

std::vector<int> WebServerConfig::getUniquePortsVec()
{
    std::set<int> uniquePortsSet;

    int port;
    std::vector<ConfigServer>::iterator it;
    for (it = this->configServerVec.begin(); it < this->configServerVec.end(); it++)
    {
        port = std::atoi(((*it).getKeyValueMap())["listen"].c_str());
        uniquePortsSet.insert(port);
    }

    std::vector<int> uniquePortsVec(uniquePortsSet.begin(), uniquePortsSet.end());
    return uniquePortsVec;
}




