/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ssian <ssian@student.42singapore.sg>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/22 10:55:00 by ssian             #+#    #+#             */
/*   Updated: 2025/02/22 10:55:01 by ssian            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#ifndef WEBSERVER_CONFIG_HPP
# define WEBSERVER_CONFIG_HPP

# include <vector>
# include "ConfigGlobal.hpp"
# include "ConfigServer.hpp"
# include <iostream>
# include <fstream>
# include <cstdlib>
# include "utilConfig.hpp"
# include <sstream>
# include <limits>



class WebServerConfig
{
    private:
        ConfigGlobal configGlobal;
        std::vector<ConfigServer> configServerVec;

        std::string readFile(std::string filePath); 
        std::string getGlobalStr(std::string configStr);
        std::vector<std::string> getServerStr(std::string configFileStr);

    public:
        WebServerConfig();
        ~WebServerConfig();
        WebServerConfig(const WebServerConfig& other);
        WebServerConfig& operator=(const WebServerConfig& other);

        ConfigGlobal getConfigGlobal();
        std::vector<ConfigServer> getConfigServerVec();
        
        void parseWebServerConfigFile(std::string configFile);

};


#endif
