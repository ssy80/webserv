/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigLocation.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ssian <ssian@student.42singapore.sg>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/22 10:49:54 by ssian             #+#    #+#             */
/*   Updated: 2025/02/22 10:49:56 by ssian            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#ifndef CONFIG_LOCATION_HPP
# define CONFIG_LOCATION_HPP

# include <map>
# include <set>
# include <cstddef>
# include <iostream>
# include "utilConfig.hpp"
# include <exception>

using std::exception;

class ConfigLocation
{
    private:
        std::map<std::string, std::string> keyValueMap;
        std::string request_path;
        std::string root;
        std::string index;
        std::string autoindex;   
        std::string methods;
        std::string redirect;
        
        bool validate();
        
    public:
        ConfigLocation();
        ~ConfigLocation();
        ConfigLocation(const ConfigLocation& other);
        ConfigLocation& operator=(const ConfigLocation& other);

        const std::map<std::string, std::string>& getKeyValueMap() const;

        void parseConfigLocation(std::string configStr);

        const std::string& getRequestPath() const;
        
        const std::string& getRoot() const;
        const std::string& getIndex() const;
        const std::string& getAutoIndex() const;
        const std::string& getMethods() const;
        const std::string& getRedirect() const;
};

#endif