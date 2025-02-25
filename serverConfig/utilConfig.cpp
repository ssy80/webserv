/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utilConfig.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ssian <ssian@student.42singapore.sg>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/22 21:58:42 by ssian             #+#    #+#             */
/*   Updated: 2025/02/22 21:58:42 by ssian            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "utilConfig.hpp"

std::string trim(const std::string& str)
{
    std::string::size_type start = str.find_first_not_of(" \t");
    if (start == std::string::npos)
        return ("");

    std::string::size_type end = str.find_last_not_of(" \t");
    return str.substr(start, end - start + 1);
}


void printMap(std::map<std::string, std::string> anyMap)
{
    std::map<std::string, std::string>::iterator it;
    for (it = anyMap.begin(); it != anyMap.end(); it++)
    {
        std::cout << it->first << " : " << it->second << std::endl; 
    }
}

void printVecStr(std::vector<std::string> vecStr)
{
    std::vector<std::string>::iterator it;
    for (it = vecStr.begin(); it < vecStr.end(); it++)
    {
        std::cout << *it << std::endl;
    } 
}

std::string removeBlock(std::string configStr, std::string startMarker, std::string endMarker)
{
    size_t startPos = configStr.find(startMarker);
    while (startPos != std::string::npos) 
    {
        size_t endPos = configStr.find(endMarker, startPos);
        if (endPos == std::string::npos)                            // No matching end marker found.
            break;
        
        endPos += endMarker.length();                               // Move endPos to include the entire end marker.
        configStr.erase(startPos, endPos - startPos);               // Erase the block from the configuration.
        startPos = configStr.find(startMarker);                     // Look for the next "[location]" marker.
    }
    return (configStr);
}


std::map<std::string, std::string> extractKeyValuePair(std::string configStr)
{
    std::map<std::string, std::string> keyValueMap;
    std::istringstream iss(configStr);
    std::string line;
    
    while (std::getline(iss, line)) 
    {
        std::string trimmedLine = trim(line);
        if (trimmedLine.empty())                                // Skip empty lines.
            continue;  
        
        std::string::size_type pos = trimmedLine.find('=');     // Find the '=' separator.
        if (pos == std::string::npos)
            continue;                                           // Skip lines that don't contain '='.
        
        std::string key = trim(trimmedLine.substr(0, pos));     // Extract key and value substrings, and trim them.
        std::string value = trim(trimmedLine.substr(pos + 1));
        
        keyValueMap[key] = value;                               // Store the key-value pair in the map.
    }

    return (keyValueMap);
}

/**
    extract multiple between block lines in to a vector for further processing
    e.g multiple [location][/location]
*/
std::vector<std::string> extractBetweenBlockVec(std::string configStr, std::string startMarker, std::string endMarker)
{
    std::vector<std::string> betweenBlockVec;
    std::vector<std::string> blockLines;
    std::string line;
    bool inBlock = false;
    std::string blockStr;
    std::istringstream iss(configStr);                   //read str

    while (std::getline(iss, line)) 
    {
        std::string trimmed = trim(line);
        if (trimmed == startMarker)                      // "[location]"
        {
            inBlock = true;
            continue;                                   // Skip the marker line.
        }

        if (trimmed == endMarker)                       // "[/location]"
        {
            inBlock = false;
            std::vector<std::string>::iterator it;
            for (it = blockLines.begin(); it != blockLines.end(); it++)
            {
                blockStr = blockStr + '\n' + (*it);
            }
            betweenBlockVec.push_back(blockStr);         //add to vector each block lines in a str
            blockStr = "";                               //clear for next block use
            blockLines.clear(); 
        }

        if (inBlock) 
        {
            blockLines.push_back(trimmed);               //add line to same block vec
        }
    }

    return (betweenBlockVec);
}

std::string extractBlock(std::string configStr, std::string startMarker, std::string endMarker)
{
    std::istringstream iss(configStr);
    std::string line;
    bool inBlock = false;
    std::vector<std::string> blockLines;
    std::string blockStr;

    while (std::getline(iss, line)) 
    {
        std::string trimmed = trim(line);
        if (trimmed == startMarker)                       //"[global]"
        {
            inBlock = true;
            continue;                                    //check next line
        }
        if (trimmed == endMarker)                        //"[/global]"
        {
            inBlock = false;
            break;                                       // Stop reading, only look for 1 block
        }
        if (inBlock) 
        {
            blockLines.push_back(trimmed);
        }
    }

    for (std::vector<std::string>::iterator it = blockLines.begin(); it != blockLines.end(); it++)   //convert lines in vector to a string 
    {
        blockStr = blockStr + '\n' + (*it);
    }

    return (blockStr);
}




int parseContentLength(const std::string& request)
{
    std::string header = "Content-Length:";
    size_t pos = request.find(header);
    if (pos == std::string::npos) {
        return -1;  // Header not found.
    }
    
    // Move position past "Content-Length:".
    pos += header.size();
    
    // Skip any whitespace after the header.
    while (pos < request.size() && (request[pos] == ' ' || request[pos] == '\t')) {
        pos++;
    }
    
    // Find the end of the line (CRLF).
    size_t end = request.find("\r\n", pos);
    if (end == std::string::npos) {
        end = request.size();
    }
    
    // Extract the substring containing the numeric value.
    std::string valueStr = request.substr(pos, end - pos);
    
    // Convert the string to an integer.
    int contentLength = std::atoi(valueStr.c_str());
    //int contentLength = stringToInt(valueStr.c_str());
    return contentLength;
}

