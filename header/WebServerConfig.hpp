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
# include <iostream>
# include <fstream>
# include <cstdlib>
# include <sstream>
# include <limits>
# include <set>
# include <exception>

# include "ConfigServer.hpp"
# include "utilConfig.hpp"
# include "ConfigGlobal.hpp"

using std::exception;

class WebServerConfig
{
    private:
        ConfigGlobal configGlobal;
        std::vector<ConfigServer> configServerVec;

        bool validate();

    public:
        WebServerConfig();
        ~WebServerConfig();
        WebServerConfig(const WebServerConfig& other);
        WebServerConfig& operator=(const WebServerConfig& other);

        const ConfigGlobal& getConfigGlobal() const;
        const std::vector<ConfigServer>& getConfigServerVec() const;

        std::vector<int> getUniquePortsVec();
        void parseWebServerConfigFile(std::string configFile);

};


#endif
