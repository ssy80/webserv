/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigServer.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daong <daong@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/22 19:53:20 by ssian             #+#    #+#             */
/*   Updated: 2025/03/22 13:54:40 by daong            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../header/ConfigServer.hpp"

ConfigServer::ConfigServer()
{
    this->listen = -1;
    this->server_name = "";
    this->max_body_size = -1;
    populateDefaultErrorPageMap();
}

ConfigServer::~ConfigServer(){}

ConfigServer::ConfigServer(const ConfigServer& other):
    configLocationVec(other.configLocationVec),
    keyValueMap(other.keyValueMap)
{
    this->listen = other.listen;
    this->server_name = other.server_name;
    this->max_body_size = other.max_body_size;
    this->errorPageMap = other.errorPageMap;
    this->defaultErrorPageMap = other.defaultErrorPageMap;
}

ConfigServer& ConfigServer::operator=(const ConfigServer& other)
{
    if (this == &other)
        return (*this);
    this->configLocationVec = other.configLocationVec;
    this->keyValueMap = other.keyValueMap;
    this->listen = other.listen;
    this->server_name = other.server_name;
    this->max_body_size = other.max_body_size;
    this->errorPageMap = other.errorPageMap;
    this->defaultErrorPageMap = other.defaultErrorPageMap;
    return (*this);
}

void ConfigServer::parseConfigServer(std::string configServerStr)
{
    std::vector<std::string> configLocationStrVec = extractBetweenBlockVec(configServerStr, "[location]", "[/location]");

    std::string errorPageStr = extractBlock(configServerStr, "[error_page]", "[/error_page]");
    this->errorPageMap = extractKeyValuePair(errorPageStr);

    configServerStr = removeBlock(configServerStr, "[error_page]", "[/error_page]");
    configServerStr = removeBlock(configServerStr, "[location]", "[/location]");
    this->keyValueMap = extractKeyValuePair(configServerStr);

    if (this->keyValueMap.size() == 0)
    {
        std::cerr << "Error: invalid config [server]" << std::endl;
        throw exception();
    }

    std::map<std::string, std::string>::iterator itServer;
    for (itServer = this->keyValueMap.begin(); itServer != this->keyValueMap.end(); itServer++)
    {

        if (itServer->first == "server_name")
        {
            this->server_name = itServer->second;
        }
        else if (itServer->first == "listen")
        {
            if (isValidPort(itServer->second))
            {
                this->listen = std::atoi(itServer->second.c_str());
            }
            else
            {
                std::cerr << "Error: invalid listen port" << std::endl;
                throw exception();
            }
        }
        else if (itServer->first == "max_body_size")
        {
            if (isValidInt(itServer->second))
            {
                this->max_body_size = std::atoi(itServer->second.c_str());
            }
            else
            {
                std::cerr << "Error: invalid max_body_size" << std::endl;
                throw exception();
            }
        }
    }
    
    std::vector<std::string>::iterator it;
    for (it = configLocationStrVec.begin(); it < configLocationStrVec.end(); it++)
    {
        ConfigLocation configLocation;
        
        configLocation.parseConfigLocation(*it);
        this->configLocationVec.push_back(configLocation);
    }

    if (!validate())
    {
        std::cerr << "Error: invalid config [server]" << std::endl;
        throw exception();
    }
}

bool ConfigServer::validate()
{
    if (this->listen == -1 || this->max_body_size == -1)
        return (false);
   
    std::vector<ConfigLocation>::iterator it;
    for (it = this->configLocationVec.begin(); it < this->configLocationVec.end(); it++)
    {
        if ((*it).getRequestPath() == "/")
        {
            return (true);
        }
    }
    return (false);
}

const int& ConfigServer::getListenPort() const
{
    return(this->listen);
}

const std::string& ConfigServer::getServerName() const
{
    return (this->server_name);
}

const int& ConfigServer::getMaxBodySize() const
{
    return (this->max_body_size);
}

const std::map<std::string, std::string>& ConfigServer::getErrorPageMap() const
{
    return (this->errorPageMap);
}
        
const std::map<std::string, std::string>& ConfigServer::getDefaultErrorPageMap() const
{
    return (this->defaultErrorPageMap);
}

const std::map<std::string, std::string>& ConfigServer::getKeyValueMap() const
{
    return (this->keyValueMap);
}

const std::vector<ConfigLocation>& ConfigServer::getConfigLocationVec() const
{
    return (this->configLocationVec);
}

void ConfigServer::populateDefaultErrorPageMap()
{
    std::string configFileStr;
    configFileStr = readFile(DEFAULT_CONFIG);

    std::string serverStr = extractBlock(configFileStr, "[server]", "[/server]");

    std::string errorPageStr = extractBlock(serverStr, "[error_page]", "[/error_page]");
    this->defaultErrorPageMap = extractKeyValuePair(errorPageStr);
}

