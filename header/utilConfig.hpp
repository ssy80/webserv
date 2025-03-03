/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utilConfig.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ssian <ssian@student.42singapore.sg>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/22 21:58:25 by ssian             #+#    #+#             */
/*   Updated: 2025/02/22 21:58:27 by ssian            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef UTILCONFIG_HPP
# define UTILCONFIG_HPP

# include <iostream>
# include <cerrno>
# include <map>
# include <vector>
# include <sstream>
# include <cstdlib>
# include <limits>
# include <fstream>
# include <sys/stat.h>


std::string trim(const std::string& str);

int stringToInt(const std::string& str);

void printMap(std::map<std::string, std::string> anyMap);

void printVecStr(std::vector<std::string> vecStr);

std::string removeBlock(std::string configStr, std::string startMarker, std::string endMarker);

std::map<std::string, std::string> extractKeyValuePair(std::string configStr);

std::vector<std::string> extractBetweenBlockVec(std::string configStr, std::string startMarker, std::string endMarker);

std::string extractBlock(std::string configStr, std::string startMarker, std::string endMarker);

int parseContentLength(const std::string& request);

std::string parseHeaderField(const std::string& request, std::string field);

bool isValidInt(std::string valueStr);

bool isValidPort(std::string portStr);

std::string readFile(std::string configFile);

std::vector<std::string> splitHost(std::string hostStr);

bool isContainIn(std::string line, std::string findStr);

std::string replacePath(const std::string& url, const std::string& requestPath, const std::string& root);


std::string readServerFile(const std::string& filePath);



class InvalidStrToIntException: public std::exception
{
    public:
        const char* what(void) const throw();
};


#endif