/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigGlobal.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ssian <ssian@student.42singapore.sg>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/22 19:52:54 by ssian             #+#    #+#             */
/*   Updated: 2025/02/22 19:52:55 by ssian            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "../header/ConfigGlobal.hpp"

ConfigGlobal::ConfigGlobal(){}

ConfigGlobal::~ConfigGlobal(){}

ConfigGlobal::ConfigGlobal(const ConfigGlobal& other): keyValueMap(other.keyValueMap)
{}

ConfigGlobal& ConfigGlobal::operator=(const ConfigGlobal& other)
{
    if (this == &other)
        return (*this);
    this->keyValueMap = other.keyValueMap;
    return (*this);
}

void ConfigGlobal::parseConfigGlobal(std::string configGlobalStr)
{
     this->keyValueMap = extractKeyValuePair(configGlobalStr);
}

std::map<std::string, std::string> ConfigGlobal::getKeyValueMap()
{
    return (this->keyValueMap);
}
