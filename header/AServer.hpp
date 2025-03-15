/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AServer.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daong <daong@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/15 09:40:56 by daong             #+#    #+#             */
/*   Updated: 2025/03/15 09:41:05 by daong            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef __ASERVER_HPP
#define __ASERVER_HPP

#include <iostream>
#include <string>
#include <fstream>
#include <map>

using std::cout;
using std::endl;
using std::string;
using std::map;
using std::ifstream;
using std::ios;
using std::noskipws;

// abstract class
class AServer{
	public:
		AServer();
		virtual ~AServer();
		virtual void startServer()=0;
};

#endif