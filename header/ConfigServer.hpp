/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigServer.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daong <daong@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/22 10:49:41 by ssian             #+#    #+#             */
/*   Updated: 2025/03/01 20:14:13 by daong            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_SERVER_HPP
# define CONFIG_SERVER_HPP

# include <vector>
# include <map>
# include "ConfigLocation.hpp"
# include <cstddef>
# include <iostream>
# include "utilConfig.hpp"
# include <sstream>

# define DEFAULT_CONFIG "./config/default.config"

class ConfigServer
{
    private:
        std::vector<ConfigLocation> configLocationVec;
        std::map<std::string, std::string> keyValueMap;

        int listen;
        std::string server_name;
        int max_body_size;
        std::map<std::string, std::string> errorPageMap;

        std::map<std::string, std::string> defaultErrorPageMap;
        
        bool validate();
        void populateDefaultErrorPageMap();
        

    public:
        ConfigServer();
        ~ConfigServer();
        ConfigServer(const ConfigServer& other);
        ConfigServer& operator=(const ConfigServer& other);

        std::map<std::string, std::string> getKeyValueMap();
        std::vector<ConfigLocation>  getConfigLocationVec();
        
        void parseConfigServer(std::string configServerStr);

        int getListenPort();
        std::string getServerName();
        int getMaxBodySize();
        std::map<std::string, std::string> getErrorPageMap();
        std::map<std::string, std::string> getDefaultErrorPageMap();
        
        

};


#endif