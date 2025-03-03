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

        std::map<std::string, std::string> getKeyValueMap();

        void parseConfigLocation(std::string configStr);
        //void setKeyValueMap(std::map<std::string, std::string> _keyValueMap);


        const std::string& getRequestPath() const;
        std::string getRoot();
        std::string getIndex();
        std::string getAutoIndex();
        std::string getMethods();
        std::string getRedirect();
        
        void populate(std::string Autoindex, std::string Index, std::string Methods, std::string Redirect, std::string Request_path, std::string Root) {
            autoindex = Autoindex;
            index = Index;
            methods = Methods;
            redirect = Redirect;
            request_path = Request_path;
            root = Root;
        }
        

    
};

#endif