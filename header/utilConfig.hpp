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
#include <iostream>
#include <cerrno>
#include <map>
#include <vector>
#include <sstream>
#include <cstdlib>


std::string trim(const std::string& str);

//int stringToInt(const std::string& str);

void printMap(std::map<std::string, std::string> anyMap);

void printVecStr(std::vector<std::string> vecStr);

std::string removeBlock(std::string configStr, std::string startMarker, std::string endMarker);

std::map<std::string, std::string> extractKeyValuePair(std::string configStr);

std::vector<std::string> extractBetweenBlockVec(std::string configStr, std::string startMarker, std::string endMarker);

std::string extractBlock(std::string configStr, std::string startMarker, std::string endMarker);

int parseContentLength(const std::string& request);