/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigLocation.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daong <daong@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/22 19:53:07 by ssian             #+#    #+#             */
/*   Updated: 2025/03/22 13:54:45 by daong            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../header/ConfigLocation.hpp"

ConfigLocation::ConfigLocation()
{
    this->request_path = "";
    this->root = "";
    this->index = "";
    this->autoindex = "";
    this->methods = "";
    this->redirect = "";
}

ConfigLocation::~ConfigLocation(){}

ConfigLocation::ConfigLocation(const ConfigLocation& other): keyValueMap(other.keyValueMap)
{
    this->request_path = other.request_path;
    this->root = other.root;
    this->index = other.index;
    this->autoindex = other.autoindex;
    this->methods = other.methods;
    this->redirect = other.redirect;
}

ConfigLocation& ConfigLocation::operator=(const ConfigLocation& other)
{
     if (this == &other)
        return (*this);
    this->keyValueMap = other.keyValueMap;
    this->request_path = other.request_path;
    this->root = other.root;
    this->index = other.index;
    this->autoindex = other.autoindex;
    this->methods = other.methods;
    this->redirect = other.redirect;
    return (*this);
}

void ConfigLocation::parseConfigLocation(std::string configLocationStr)
{
    this->keyValueMap = extractKeyValuePair(configLocationStr);
    
    std::map<std::string, std::string>::iterator it;
    for (it = this->keyValueMap.begin(); it != this->keyValueMap.end(); it++)
    {
        if (it->first == "request_path")
            this->request_path = it->second;
        else if (it->first == "root")
            this->root = it->second;
        else if (it->first == "index")
            this->index = it->second;
        else if (it->first == "autoindex")
            this->autoindex = it->second;
        else if (it->first == "methods")
            this->methods = it->second;
        else if (it->first == "redirect")
            this->redirect = it->second;
    }
    
    if (!validate())
    {
        std::cerr << "Error: invalid [location] config" << std::endl;
        throw exception();
    }
}

bool ConfigLocation::validate()
{
    if (this->autoindex != "on" && this->autoindex != "off")
        return (false);
    if (this->request_path == "" || this->root == "" || this->index == "" || this->methods == "")
        return (false);
    return (true);
}

const std::map<std::string, std::string>& ConfigLocation::getKeyValueMap() const
{
    return (this->keyValueMap);
}

const std::string& ConfigLocation::getRequestPath() const
{
    return (this->request_path);
}

const std::string& ConfigLocation::getRoot() const
{
    return (this->root);
}

const std::string& ConfigLocation::getIndex() const
{
    return (this->index);
}

const std::string& ConfigLocation::getAutoIndex() const
{
    return (this->autoindex);
}

const std::string& ConfigLocation::getMethods() const
{
    return (this->methods);   
}

const std::string& ConfigLocation::getRedirect() const
{
    return (this->redirect);   
}