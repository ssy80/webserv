/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigGlobal.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daong <daong@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/22 19:52:54 by ssian             #+#    #+#             */
/*   Updated: 2025/03/09 13:36:40 by daong            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../header/ConfigGlobal.hpp"

ConfigGlobal::ConfigGlobal(){}

ConfigGlobal::~ConfigGlobal(){}

ConfigGlobal::ConfigGlobal(const ConfigGlobal& other)
: keyValueMap(other.keyValueMap), timeout(other.timeout), max_events(other.max_events), upload_directory(other.upload_directory)
{}

ConfigGlobal& ConfigGlobal::operator=(const ConfigGlobal& other)
{
    if (this == &other)
        return (*this);
    this->keyValueMap = other.keyValueMap;
    this->timeout = other.timeout;
    this->max_events = other.max_events;
    this->upload_directory = other.upload_directory;
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

    std::cout << "Checking config keyValueMap contents:\n";
    for (it = this->keyValueMap.begin(); it != this->keyValueMap.end(); ++it) {
        std::cout << it->first << " = " << it->second << "\n";
    }


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

    it = this->keyValueMap.find("upload_directory");
    if (it != this->keyValueMap.end())
    {
        this->upload_directory = it->second;
    }
    else
    {
        std::cerr << "Erorr: invalid config [global]" << std::endl;
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

const std::string& ConfigGlobal::getUploadDirectory() const
{
    std::cout << "Getting upload" << this->upload_directory << std::endl;
    return (this->upload_directory);
}
