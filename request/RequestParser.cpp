#include "../header/RequestParser.hpp"
#include "../header/Response.hpp"

Request RequestParser::parseRequest(const string& rawRequest) {
	Request request;
	istringstream stream(rawRequest);
	string line;
	// Parse the request line
	if (getline(stream, line)) {
		istringstream requestLine(line);
		requestLine >> request.method >> request.url >> request.version;
	}
	// Parse headers
	string contentType;
	while (getline(stream, line) && !line.empty()) {
    if (line == "\r" || line == "\n" || line == CLRF) break;
		size_t colonPos = line.find(':');
		if (colonPos != string::npos) {
			string headerName = line.substr(0, colonPos);
			string headerValue = line.substr(colonPos + 1);
			headerValue.erase(0, headerValue.find_first_not_of(" \t"));
			request.headers[headerName] = headerValue;
			// Capture Content-Type for multipart/form-data parsing
      if (headerName == "Content-Type") 
        contentType = headerValue;
    }
	}
  // If Content-Type is multipart/form-data, parse the body accordingly
  if (!contentType.empty() && contentType.find("multipart/form-data") != string::npos)
    parseMultipartFormData(stream, request, contentType);
  // parse body part
  while (getline(stream, line))
    request.files["body"] += line;
	return request;
}

void RequestParser::parsePartFormData(istringstream& stream, Request& request, const string& endpt){
  string line;
  int i=0;
  bool skip = false;
  string name="default";
  string contentDisposition;
  string content;
  size_t pos;
  while (getline(stream, line)) {
    if (line.find(endpt) != string::npos) break;
    // parsing of sub header
    if (i==0){
      pos = line.find("name=");
      if (pos != string::npos){
        pos += 6;
        name.clear();
        while (line[pos] != '"')
          name+=line.at(pos++);
        // parsing contentdisposition
        // pos = line.find("Content-Disposition: ");
        // if (pos != string::npos){
        //   pos+= 21;
        //   while (line[pos] != ';')
        //     contentDisposition+=line[pos++];
        // }
        // parsing filename into formfield
        pos = line.find("filename=");
        if (pos != string::npos){
          pos+= 10;
          while (line[pos] != '"')
            contentDisposition+=line[pos++];
        }
      }
      skip = true;
      i++;
      continue;
    }
    // skip the next line
    if (skip){
      skip = false;
      i++;
      continue;
    }
    // if got multipart/form-data
    if (!content.empty() && content.find("multipart/form-data") != string::npos){
      parseMultipartFormData(stream, request, content);
      continue;
    }
    // getting the content
      content += line + '\n';
      i++;
  }
  if (content[content.size()-1] == '\n')
    content.erase(content.end()-1);
  request.files[name] = content;
  if (!contentDisposition.empty())
    request.formFields[name] = contentDisposition;
}

string RequestParser::extractFilename(const std::string& contentDisposition) {
	std::cout << "starting extract" << std::endl;
	string searchStr = "filename=";
	size_t filenamePos = contentDisposition.find(searchStr);
	std::cout << filenamePos << std::endl;
	if (filenamePos != string::npos) {
		size_t startPos = contentDisposition.find("\"", filenamePos) + 1;
		std::cout << "startPos" << startPos << std::endl;
		size_t endPos = contentDisposition.find("\"", startPos);
		std::cout << "endPos" << endPos << std::endl;
		return contentDisposition.substr(startPos, endPos - startPos);
	}
	return "";
}

void RequestParser::parseMultipartFormData(istringstream& stream, Request& request, const string& contentType) {
  size_t boundaryPos = contentType.find("boundary=");
  // No boundary found
  if (boundaryPos == string::npos) 
    return; 
  string boundary = contentType.substr(boundaryPos + 9);
  // remove whitespace
  if (!boundary.empty() && boundary[boundary.size()-1] == '\r')
    boundary.erase(boundary.end()-1);
  string endpt = boundary + "--";
  string line;
  string partData;
  while (getline(stream, line)) {
    // boundary found
    if (line.find(boundary) != string::npos) 
      parsePartFormData(stream, request, endpt);
  }
}

