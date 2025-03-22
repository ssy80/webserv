/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daong <daong@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/15 09:41:45 by daong             #+#    #+#             */
/*   Updated: 2025/03/22 13:56:05 by daong            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef __REQUEST_HPP
#define __REQUEST_HPP

#include <iostream>
#include <string>
#include <map>
#include <sstream>
#include <vector>
#include "AServer.hpp"

using std::map;
using std::vector;

class Request {

	public:
		string method;
		string url;
		string version;
		map<string, string> headers;
    	map<string, string> formFields;
    	map<string, string> files;

		Request();
		~Request();
		void print();
};

#endif