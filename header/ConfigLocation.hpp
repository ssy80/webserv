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
        
    public:
        ConfigLocation();
        ~ConfigLocation();
        ConfigLocation(const ConfigLocation& other);
        ConfigLocation& operator=(const ConfigLocation& other);

        std::map<std::string, std::string> getKeyValueMap();

        void parseConfigLocation(std::string configStr);
        void setKeyValueMap(std::map<std::string, std::string> _keyValueMap);

    
};

#endif