#include "../header/RequestParser.hpp"

Request RequestParser::parseRequest(const string& rawRequest) {
	Request request;
	istringstream stream(rawRequest);
	string line;

	// Parse the request line (e.g., "GET /index.html HTTP/1.1")
	if (getline(stream, line)) {
		istringstream requestLine(line);
		requestLine >> request.method >> request.url >> request.version;
	}

	// Parse the headers (key: value pairs)
	while (getline(stream, line) && !line.empty()) {
		size_t colonPos = line.find(':');
		if (colonPos != string::npos) {
			string headerName = line.substr(0, colonPos);
			string headerValue = line.substr(colonPos + 1);
			// Trim leading whitespace from the value
			headerValue.erase(0, headerValue.find_first_not_of(" \t"));
			request.headers[headerName] = headerValue;
		}
	}

	// Parse the body if present (after an empty line)
	ostringstream bodyStream;
	while (getline(stream, line))
		bodyStream << line << "\n";
	request.body = bodyStream.str();
	return request;
}