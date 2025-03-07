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

ConfigGlobal::ConfigGlobal(const ConfigGlobal& other)
: keyValueMap(other.keyValueMap), timeout(other.timeout), max_events(other.max_events)
{}

ConfigGlobal& ConfigGlobal::operator=(const ConfigGlobal& other)
{
    if (this == &other)
        return (*this);
    this->keyValueMap = other.keyValueMap;
    this->timeout = other.timeout;
    this->max_events = other.max_events;
    return (*this);
}

void ConfigGlobal::parseConfigGlobal(std::string configGlobalStr)
{
    this->keyValueMap = extractKeyValuePair(configGlobalStr);

    if (this->keyValueMap.size() == 0)
    {
        std::cerr << "Error: invalid config [global]" << std::endl;
        exit(1);
    }
     
    std::map<std::string, std::string>::iterator it;

    it = this->keyValueMap.find("timeout");
    if (it != this->keyValueMap.end())
    {
        if (isValidInt(it->second))
        {
            this->timeout = std::atoi(it->second.c_str());
        }
        else
        {
            std::cerr << "Error: invalid timeout" << std::endl;
            exit(1);
        }
    }
    else
    {
        std::cerr << "Error: invalid config [global]" << std::endl;
        exit(1);
    }

    it = this->keyValueMap.find("max_events");
    if (it != this->keyValueMap.end())
    {
        if (isValidInt(it->second))
        {
            this->max_events = std::atoi(it->second.c_str());
        }
        else
        {
            std::cerr << "Error: invalid max_events" << std::endl;
            exit(1);
        }
        if (this->max_events <= 0)
        {
            std::cerr << "Error: invalid max_events" << std::endl;
            exit(1);
        }
    }
    else
    {
        std::cerr << "Error: invalid config [global]" << std::endl;
        exit(1);
    }
}

const std::map<std::string, std::string>& ConfigGlobal::getKeyValueMap() const
{
    return (this->keyValueMap);
}

const int& ConfigGlobal::getTimeout() const
{
    return (this->timeout);
}

const int& ConfigGlobal::getMaxEvents() const
{
    return (this->max_events);
}
