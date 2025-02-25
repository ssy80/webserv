/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigLocation.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ssian <ssian@student.42singapore.sg>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/22 19:53:07 by ssian             #+#    #+#             */
/*   Updated: 2025/02/22 19:53:08 by ssian            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "ConfigLocation.hpp"

ConfigLocation::ConfigLocation(){}

ConfigLocation::~ConfigLocation(){}

ConfigLocation::ConfigLocation(const ConfigLocation& other): keyValueMap(other.keyValueMap)
{}

ConfigLocation& ConfigLocation::operator=(const ConfigLocation& other)
{
     if (this == &other)
        return (*this);
    this->keyValueMap = other.keyValueMap;
    return (*this);
}

void ConfigLocation::parseConfigLocation(std::string configLocationStr)
{
    std::map<std::string, std::string> extractedKeyValueMap = extractKeyValuePair(configLocationStr);
    std::map<std::string, std::string>::iterator it;
    for (it = extractedKeyValueMap.begin(); it != extractedKeyValueMap.end(); it++)
    {
        this->keyValueMap[it->first] = it->second;
    }
    //this->keyValueMap = extractKeyValuePair(configLocationStr);
}

std::map<std::string, std::string> ConfigLocation::getKeyValueMap()
{
    return (this->keyValueMap);
}

void ConfigLocation::setKeyValueMap(std::map<std::string, std::string> _keyValueMap)
{
     this->keyValueMap = _keyValueMap;
}