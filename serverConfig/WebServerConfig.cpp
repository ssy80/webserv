/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServerConfig.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daong <daong@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/22 19:53:45 by ssian             #+#    #+#             */
/*   Updated: 2025/03/22 13:53:16 by daong            ###   ########.fr       */
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
    configFileStr = readFile(configFile);

    std::string globalStr = extractBlock(configFileStr, "[global]", "[/global]");
    this->configGlobal.parseConfigGlobal(globalStr); 

    std::vector<std::string> configServerStrVec = extractBetweenBlockVec(configFileStr, "[server]", "[/server]");
    if (configServerStrVec.size() == 0)
    {
        std::cerr << "Error: invalid config [server]" << std::endl;
        throw exception();
    }

    std::vector<std::string>::iterator it;
    for (it = configServerStrVec.begin(); it < configServerStrVec.end(); it++)    
    {
        ConfigServer configServer;
        configServer.parseConfigServer(*it);
        this->configServerVec.push_back(configServer);                            
    }

    if (!validate())
    {
        std::cerr << "Error: invalid config [server]" << std::endl;
        throw exception();
    }
}

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

const ConfigGlobal& WebServerConfig::getConfigGlobal() const
{
    return (this->configGlobal);
}

const std::vector<ConfigServer>& WebServerConfig::getConfigServerVec() const
{
    return (this->configServerVec);
}

std::vector<int> WebServerConfig::getUniquePortsVec()
{
    std::set<int> uniquePortsSet;

    std::vector<ConfigServer>::iterator it;
    for (it = this->configServerVec.begin(); it < this->configServerVec.end(); it++)
    {
        uniquePortsSet.insert((*it).getListenPort());
    }

    std::vector<int> uniquePortsVec(uniquePortsSet.begin(), uniquePortsSet.end());
    return uniquePortsVec;
}
