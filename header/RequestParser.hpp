/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daong <daong@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/15 09:41:50 by daong             #+#    #+#             */
/*   Updated: 2025/03/15 09:41:52 by daong            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef __REQUESTPARSER_HPP
#define __REQUESTPARSER_HPP

#include "Request.hpp"

using std::istringstream;
using std::getline;
using std::ostringstream;

class RequestParser{
	public:
		static Request parseRequest(const string& rawRequest);
	
	private:
		static void parseMultipartFormData(istringstream& stream, Request& request, const string& contentType);
		static string extractFilename(const string& contentDisposition);
		static void parsePartFormData(istringstream& stream, Request& request, const string& endpt);
};

#endif