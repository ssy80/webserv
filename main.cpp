/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ssian <ssian@student.42singapore.sg>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/22 10:39:47 by ssian             #+#    #+#             */
/*   Updated: 2025/02/22 10:39:56 by ssian            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include <iostream>
#include "WebServerConfig.hpp"
#include "GlobalServer.hpp"


int main(int argc, char**argv)
{
    (void) argv;
    if (argc != 2)
    {
        std::cout << "need config file" << std::endl;
        return (0);
    }
    else
    {
        WebServerConfig webServerConfig;
        webServerConfig.parseWebServerConfigFile(argv[1]);                                                    //parse the config file

        ConfigGlobal configGlobal = webServerConfig.getConfigGlobal();                                        //object for [global] [/global] 
        std::vector<ConfigServer> configServerVec = webServerConfig.getConfigServerVec();                     //objects for [server] [/server] in vector 
        
        std::cout << "[global]" << std::endl;
        printMap(configGlobal.getKeyValueMap());                                                             // key/value map for [global] [/global] 
        std::cout << std::endl;
        
        std::vector<ConfigServer>::iterator it;
        for (it = configServerVec.begin(); it < configServerVec.end(); it++)                                 //loop server vector 
        {
            std::cout << "[server]" << std::endl;                                                           
            ConfigServer configServer = (*it);                                                               // get the sever object
            printMap(configServer.getKeyValueMap());                                                        // get key/value for each server
            std::cout << std::endl;

            std::vector<ConfigLocation> configLocationVec = configServer.getConfigLocationVec();           //objects for [location][/location] in vector for each [server]
        
            std::vector<ConfigLocation>::iterator locationIt;
            for (locationIt = configLocationVec.begin(); locationIt <  configLocationVec.end(); locationIt++)
            {
                std::cout << "[location]" << std::endl;
                ConfigLocation configLocation = (*locationIt);
                printMap(configLocation.getKeyValueMap());                                                    //key/value for each [location]
                std::cout << std::endl;
            }
        }

        GlobalServer globalServer(webServerConfig);
        globalServer.startServer();


    }
    return (0);
}