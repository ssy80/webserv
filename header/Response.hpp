/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daong <daong@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/15 09:41:59 by daong             #+#    #+#             */
/*   Updated: 2025/03/22 13:47:00 by daong            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include "AServer.hpp"
#include <sstream>

using std::stringstream;

#define CLRF "\r\n"
#define HTTPVERSION "HTTP/1.1"

// info
#define SC100 "100 Continue"
#define SC101 "101 Switching Protocols"
#define SC103 "103 Early Hints"

// success
#define SC200 "200 OK"
#define SC201 "201 Created"
#define SC202 "202 Accepted"
#define SC203 "203 Non-Authoritative Information"
#define SC204 "204 No Content"
#define SC205 "205 Reset Content"
#define SC206 "206 Partial Content"

// redirect
#define SC300 "300 Multiple Choices"
#define SC301 "301 Moved Permanently"
#define SC302 "302 Found"
#define SC303 "303 See Other"
#define SC304 "304 Not Modified"
#define SC307 "307 Temporary Redirect"
#define SC308 "308 Permanent Redirect"

// client error
#define SC400 "400 Bad Request"
#define SC401 "401 Unauthorized"
#define SC402 "402 Payment Required"
#define SC403 "403 Forbidden"
#define SC404 "404 Not Found"
#define SC405 "405 Method Not Allowed"
#define SC406 "406 Not Acceptable"
#define SC407 "407 Proxy Authentication Required"
#define SC408 "408 Request Timeout"
#define SC409 "409 Conflict"
#define SC410 "410 Gone"
#define SC411 "411 Length Required"
#define SC412 "412 Precondition Failed"
#define SC413 "413 Content Too Large"
#define SC414 "414 URI Too Long"
#define SC415 "415 Unsupported Media Type"
#define SC416 "416 Range Not Satisfiable"
#define SC417 "417 Expectation Failed"
#define SC418 "418 I'm a teapot"
#define SC421 "421 Misdirected Request"
#define SC422 "422 Unprocessable Content"
#define SC426 "426 Upgrade Required"
#define SC428 "428 Precondition Required"
#define SC429 "429 Too Many Requests"
#define SC431 "431 Request Header Fields Too Large"

// server error
#define SC500 "500 Internal Server Error"
#define SC501 "501 Not Implemented"
#define SC502 "502 Bad Gateway"
#define SC503 "503 Service Unavailable"
#define SC504 "504 Gateway Timeout"
#define SC505 "505 HTTP Version Not Supported"

class Response{
	private:
		string statusCode;
		string contentType;
		int contentLength;
		string misc;

	public:
		class ResBuilder {
			public:
				string statusCode;
				string contentType;
				int contentLength;
				string misc;

				ResBuilder();
				ResBuilder* sc(const string& code);
				ResBuilder* ct(const string& type);
				ResBuilder* cl(int len);
				ResBuilder* mc(const string& key, const string& value);
				~ResBuilder();
				Response build();
		};

		Response(ResBuilder builder);
    	~Response();
		string toString();
};

#endif