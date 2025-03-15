/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigServer.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ssian <ssian@student.42singapore.sg>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/22 19:53:20 by ssian             #+#    #+#             */
/*   Updated: 2025/02/22 19:53:20 by ssian            ###   ########.fr       */
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

/** 1) get between [location][/location]
    2) parse to ConfigLocation, add each ConfigLocation to this->configLocationVec 
    3) remove all [location][/location] ftom configServerStr 
    4) extract key/value to ConfigServer server's keyValueMap*/
void ConfigServer::parseConfigServer(std::string configServerStr)
{
    //get location block
    std::vector<std::string> configLocationStrVec = extractBetweenBlockVec(configServerStr, "[location]", "[/location]");

    //get errorPage block
    std::string errorPageStr = extractBlock(configServerStr, "[error_page]", "[/error_page]");
    this->errorPageMap = extractKeyValuePair(errorPageStr);


    //remove location block, error_page block, only left server values;
    configServerStr = removeBlock(configServerStr, "[error_page]", "[/error_page]");
    configServerStr = removeBlock(configServerStr, "[location]", "[/location]");
    this->keyValueMap = extractKeyValuePair(configServerStr);

    if (this->keyValueMap.size() == 0)
    {
        std::cerr << "Error: invalid config [server]" << std::endl;
        throw exception();
    }

    //set attributes
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
    
    //parse configLocation
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

/* check location must have at least one / root location  */
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

/* read default config file first [server] [error_page] */
void ConfigServer::populateDefaultErrorPageMap()
{
    std::string configFileStr;
    configFileStr = readFile(DEFAULT_CONFIG);

    std::string serverStr = extractBlock(configFileStr, "[server]", "[/server]");

    std::string errorPageStr = extractBlock(serverStr, "[error_page]", "[/error_page]");
    this->defaultErrorPageMap = extractKeyValuePair(errorPageStr);
}

