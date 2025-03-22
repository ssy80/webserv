/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daong <daong@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/15 09:42:26 by daong             #+#    #+#             */
/*   Updated: 2025/03/22 13:55:34 by daong            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../header/Response.hpp"

Response::ResBuilder::ResBuilder():
statusCode(SC200),
contentType(""),
contentLength(0),
misc("")
{};

Response::ResBuilder::~ResBuilder()
{};

Response::ResBuilder* Response::ResBuilder::sc(const string& code){
	this->statusCode = code;
	return this;
};

Response::ResBuilder* Response::ResBuilder::ct(const string& type){
	this->contentType = type;
	return this;
};

Response::ResBuilder* Response::ResBuilder::cl(int len){
	this->contentLength = len;
	return this;
}

Response::ResBuilder* Response::ResBuilder::mc(const string& key, const string& value){
	this->misc += key + ": " + value + CLRF;
	return this;
}

Response Response::ResBuilder::build(){
	return Response(*this);
}

Response::Response(ResBuilder builder):
statusCode(builder.statusCode),
contentType(builder.contentType),
contentLength(builder.contentLength),
misc(builder.misc)
{}

Response::~Response()
{};

string Response::toString(){
	stringstream ss;
	ss << HTTPVERSION << " " <<this->statusCode << CLRF;
	if (!this->contentType.empty())
		ss << this->contentType << CLRF;
	if (!this->misc.empty())
		ss << this->misc;
	ss << "Content-Length: " <<this->contentLength << CLRF << CLRF;
	return ss.str();
}


