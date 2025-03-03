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

//should not have listen, server_name, max_body_size
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
        exit(1);
    }
}

bool ConfigLocation::validate()
{
    if (this->request_path == "" || this->root == "" || this->index == "" || this->autoindex == "" || this->methods == "")
        return (false);
    return (true);
}

std::map<std::string, std::string> ConfigLocation::getKeyValueMap()
{
    return (this->keyValueMap);
}

const std::string& ConfigLocation::getRequestPath() const
{
    return (this->request_path);
}

std::string ConfigLocation::getRoot()
{
    return (this->root);
}

std::string ConfigLocation::getIndex()
{
    return (this->index);
}

std::string ConfigLocation::getAutoIndex()
{
    return (this->autoindex);
}

std::string ConfigLocation::getMethods()
{
    return (this->methods);   
}

std::string ConfigLocation::getRedirect()
{
    return (this->redirect);   
}