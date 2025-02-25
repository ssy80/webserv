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
#include "ConfigServer.hpp"

ConfigServer::ConfigServer(){}

ConfigServer::~ConfigServer(){}

ConfigServer::ConfigServer(const ConfigServer& other):
    configLocationVec(other.configLocationVec),
    keyValueMap(other.keyValueMap)
{}

ConfigServer& ConfigServer::operator=(const ConfigServer& other)
{
    if (this == &other)
        return (*this);
    this->configLocationVec = other.configLocationVec;
    this->keyValueMap = other.keyValueMap;
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

    //remove location block, only left server values;
    configServerStr = removeBlock(configServerStr, "[location]", "[/location]");
    this->keyValueMap = extractKeyValuePair(configServerStr);


    
    std::vector<std::string>::iterator it;
    for (it = configLocationStrVec.begin(); it < configLocationStrVec.end(); it++)
    {
        ConfigLocation configLocation;
        configLocation.setKeyValueMap(this->keyValueMap);
        
        configLocation.parseConfigLocation(*it);
        this->configLocationVec.push_back(configLocation);
    }

    
}
        

/*std::vector<std::string> ConfigServer::getLocationStr(std::string configServerStr)
{
    std::vector<std::string> configLocationStrVec;

    // Use an istringstream to read the configuration string line by line.
    std::istringstream iss(configServerStr);
    std::string line;
    bool inLocation = false;
    std::vector<std::string> locationLines;
    std::string locationStr;

    while (std::getline(iss, line)) 
    {
        std::string trimmed = trim(line);
        if (trimmed == "[location]") 
        {
            inLocation = true;
            continue; // Skip the marker line.
        }
        if (trimmed == "[/location]") 
        {
            inLocation = false;
            //break;    // Stop reading after the global block.
            for (std::vector<std::string>::iterator it = locationLines.begin(); it != locationLines.end(); it++)
            {
                locationStr = locationStr + '\n' + (*it);
            }
            configLocationStrVec.push_back(locationStr);
            locationStr = "";
            locationLines.clear();
        }
        if (inLocation) 
        {
            locationLines.push_back(trimmed);
        }
    }

    return (configLocationStrVec);
}*/


std::map<std::string, std::string> ConfigServer::getKeyValueMap()
{
    return (this->keyValueMap);
}

std::vector<ConfigLocation> ConfigServer::getConfigLocationVec()
{
    return (this->configLocationVec);
}