/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daong <daong@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/22 10:39:47 by ssian             #+#    #+#             */
/*   Updated: 2025/03/03 11:32:07 by daong            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header/WebServerConfig.hpp"
#include "header/GlobalServer.hpp"
#include "header/ReqServer.hpp"
#include "header/YSServer.hpp"


void displayParseConfig(WebServerConfig webServerConfig)
{
    ConfigGlobal configGlobal = webServerConfig.getConfigGlobal();                                        //object for [global] [/global] 
    std::vector<ConfigServer> configServerVec = webServerConfig.getConfigServerVec();                     //objects for [server] [/server] in vector 
    
    cout << "[global]" << endl;
    printMap(configGlobal.getKeyValueMap());                                                             // key/value map for [global] [/global] 
    cout << endl;
    
    std::vector<ConfigServer>::iterator it;
    for (it = configServerVec.begin(); it < configServerVec.end(); it++)                                 //loop server vector 
    {
        cout << "[server]" << endl;                                                           
        ConfigServer configServer = (*it);                                                               // get the sever object
        //printMap(configServer.getKeyValueMap());                                                        // get key/value for each server
        std::cout << "listen:" << configServer.getListenPort() << std::endl;
        std::cout << "max_body_size:" << configServer.getMaxBodySize() << std::endl;
        std::cout << "server_name:" << configServer.getServerName() << std::endl;
        printMap(configServer.getErrorPageMap());
        cout << "[default error page]" << endl;
        printMap(configServer.getDefaultErrorPageMap());
        cout << endl;   

        std::vector<ConfigLocation> configLocationVec = configServer.getConfigLocationVec();           //objects for [location][/location] in vector for each [server]
    
        std::vector<ConfigLocation>::iterator locationIt;
        for (locationIt = configLocationVec.begin(); locationIt <  configLocationVec.end(); locationIt++)
        {
            cout << "[location]" << endl;
            ConfigLocation configLocation = (*locationIt);
            //printMap(configLocation.getKeyValueMap());                                                    //key/value for each [location]
            std::cout << "autoindex:" << configLocation.getAutoIndex() << std::endl;
            std::cout << "index:" << configLocation.getIndex() << std::endl;
            std::cout << "methods:" << configLocation.getMethods() << std::endl;
            std::cout << "redirect:" << configLocation.getRedirect() << std::endl;
            std::cout << "request_path:" << configLocation.getRequestPath() << std::endl;
            std::cout << "root:" << configLocation.getRoot() << std::endl;
            cout << endl;
        }
    }
}

int main(int argc, char**argv)
{
    std::string configFile = "./config/default.config";

    if (argc > 2)
    {
        cout << "Error: invalid number of arguments" << endl;
        return (0);
    }
    else if (argc == 2)
    {
        configFile = argv[1];
    }

    WebServerConfig webServerConfig;
    webServerConfig.parseWebServerConfigFile(configFile);                                                    //parse the config file

    displayParseConfig(webServerConfig);

    // AServer *globalServer = new GlobalServer(webServerConfig);
    AServer *globalServer = new YSServer();
    //AServer *globalServer = new ReqServer();
    globalServer->startServer();
    delete globalServer;
    
    return (0);
}