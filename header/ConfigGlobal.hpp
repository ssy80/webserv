/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigGlobal.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daong <daong@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/22 10:49:26 by ssian             #+#    #+#             */
/*   Updated: 2025/03/09 10:20:06 by daong            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_GLOBAL_HPP
# define CONFIG_GLOBAL_HPP

# include <map>
# include <cstddef>
# include <iostream>
# include <sstream>
# include <exception>

# include "utilConfig.hpp"

using std::exception;

class ConfigGlobal
{
    private:
        std::map<std::string, std::string> keyValueMap;

        int timeout;
        int max_events;
        std::string upload_directory;

    public:
        ConfigGlobal();
        ~ConfigGlobal();
        ConfigGlobal(const ConfigGlobal& other);
        ConfigGlobal& operator=(const ConfigGlobal& other);

        const std::map<std::string, std::string>& getKeyValueMap() const;
        
        void parseConfigGlobal(std::string configGlobalStr);

        const int& getTimeout() const;
        const int& getMaxEvents() const;
        const std::string& getUploadDirectory() const;

        class ParseErrException : public exception	{
			public:
				virtual const char *what() const throw();
		};
};

#endif