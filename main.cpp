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

int main(int argc, char**argv)
{
    (void) argv;
    if (argc != 2)
    {
        cout << "need config file" << endl;
        return (0);
    }
    else
    {
        WebServerConfig webServerConfig;
        webServerConfig.parseWebServerConfigFile(argv[1]);                                                    //parse the config file

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
            printMap(configServer.getKeyValueMap());                                                        // get key/value for each server
            cout << endl;

            std::vector<ConfigLocation> configLocationVec = configServer.getConfigLocationVec();           //objects for [location][/location] in vector for each [server]
        
            std::vector<ConfigLocation>::iterator locationIt;
            for (locationIt = configLocationVec.begin(); locationIt <  configLocationVec.end(); locationIt++)
            {
                cout << "[location]" << endl;
                ConfigLocation configLocation = (*locationIt);
                printMap(configLocation.getKeyValueMap());                                                    //key/value for each [location]
                cout << endl;
            }
        }

				//AServer *globalServer = new GlobalServer(webServerConfig);
				 AServer *globalServer = new YSServer();
				// AServer *globalServer = new ReqServer();
        globalServer->startServer();
				delete globalServer;
    }
    return (0);
}