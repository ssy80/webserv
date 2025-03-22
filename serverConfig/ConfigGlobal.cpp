/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigGlobal.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daong <daong@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/22 19:52:54 by ssian             #+#    #+#             */
/*   Updated: 2025/03/22 13:54:50 by daong            ###   ########.fr       */
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
        throw ParseErrException();
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
            throw ParseErrException();
        }
    }
    else
    {
        std::cerr << "Error: invalid config [global]" << std::endl;
        throw ParseErrException();
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
            throw ParseErrException();
        }
        if (this->max_events <= 0)
        {
            std::cerr << "Error: invalid max_events" << std::endl;
            throw ParseErrException();
        }
    }
    else
    {
        std::cerr << "Error: invalid config [global]" << std::endl;
        throw ParseErrException();
    }

    it = this->keyValueMap.find("upload_directory");
    if (it != this->keyValueMap.end())
    {
        this->upload_directory = it->second;
    }
    else
    {
        std::cerr << "Erorr: invalid config [global]" << std::endl;
        throw ParseErrException();
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
    return (this->upload_directory);
}

const char *ConfigGlobal::ParseErrException::what() const throw(){
	return "Parsing error in config file";
}
