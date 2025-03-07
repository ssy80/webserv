/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigGlobal.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ssian <ssian@student.42singapore.sg>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/22 10:49:26 by ssian             #+#    #+#             */
/*   Updated: 2025/02/22 10:49:28 by ssian            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#ifndef CONFIG_GLOBAL_HPP
# define CONFIG_GLOBAL_HPP

# include <map>
# include <cstddef>
# include <iostream>
# include <sstream>
# include "utilConfig.hpp"


class ConfigGlobal
{
    private:
        std::map<std::string, std::string> keyValueMap;

        int timeout;
        int max_events;

    public:
        ConfigGlobal();
        ~ConfigGlobal();
        ConfigGlobal(const ConfigGlobal& other);
        ConfigGlobal& operator=(const ConfigGlobal& other);

        const std::map<std::string, std::string>& getKeyValueMap() const;
        
        void parseConfigGlobal(std::string configGlobalStr);

        const int& getTimeout() const;
        const int& getMaxEvents() const;

};



#endif