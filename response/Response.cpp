#include "../header/Response.hpp"

Response::ResBuilder::ResBuilder():
statusCode(SC200),
contentType(""),
contentLength(0),
misc("")
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

string Response::toString(){
	stringstream ss;
	ss << HTTPVERSION << " " <<this->statusCode << CLRF;
	// content type
	if (!this->contentType.empty())
		ss << this->contentType << CLRF;
	// extra header stuff
	if (!this->misc.empty())
		ss << this->misc;
	// content length
	ss << "Content-Length: " <<this->contentLength << CLRF << CLRF;
	return ss.str();
}


