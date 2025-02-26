#include "../header/Response.hpp"

Response::ResBuilder::ResBuilder():
statusCode(SC200),
contentType(""),
contentLength(0)
{};

Response::ResBuilder* Response::ResBuilder::sc(const string code){
	this->statusCode = code;
	return this;
};

Response::ResBuilder* Response::ResBuilder::ct(const string type){
	this->contentType = type;
	return this;
};

Response::ResBuilder* Response::ResBuilder::cl(int len){
	this->contentLength = len;
	return this;
}

Response Response::ResBuilder::build(){
	return Response(*this);
}

Response::Response(ResBuilder builder):
statusCode(builder.statusCode),
contentType(builder.contentType),
contentLength(builder.contentLength)
{}

string Response::toString(){
	stringstream ss;
	ss << HTTPVERSION << " " <<this->statusCode << CLRF;
	if (!this->contentType.empty())
		ss << this->contentType << CLRF;
	ss << "Content-Length: " <<this->contentLength << CLRF << CLRF;
	return ss.str();
}


