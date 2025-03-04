#include "../header/RequestParser.hpp"

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
	return request;
}

void RequestParser::parseMultipartFormData(std::istringstream& stream, Request& request, const std::string& contentType) {
	// Extract boundary from Content-Type
	size_t boundaryPos = contentType.find("boundary=");
	if (boundaryPos == std::string::npos) {
		std::cerr << "No boundary found in Content-Type" << endl;
		return;
	}
	string boundary = "--" + contentType.substr(boundaryPos + 9); // Skip "boundary="
	string part;
	string currentHeader;
	ostringstream bodyStream;

	while (std::getline(stream, part)) {
		// If part starts with the boundary, it indicates the start of a new part
		if (part.find(boundary) != string::npos) {
			// Process the previous part
			if (!bodyStream.str().empty()) 
				processFormPart(bodyStream.str(), request);
			bodyStream.str("");  // Clear body stream for the next part
		}
		else
			bodyStream << part << "\n";  // Add part content to the body stream
	}
	// Process the last part (if any)
	if (!bodyStream.str().empty()) 
		processFormPart(bodyStream.str(), request);
}

void RequestParser::processFormPart(const string& part, Request& request) {
	// Find the headers (e.g., Content-Disposition, Content-Type)
	istringstream partStream(part);
	string headerLine;
	string contentDisposition;
	string contentType;
	ostringstream bodyStream;

	while (std::getline(partStream, headerLine) && !headerLine.empty()) {
		if (headerLine.find("Content-Disposition") != string::npos) {
			contentDisposition = headerLine;
		} else if (headerLine.find("Content-Type") != string::npos) {
			contentType = headerLine;
		}
	}

	// Now, collect the body part (after headers)
	string bodyLine;
	while (getline(partStream, bodyLine)) 
		bodyStream << bodyLine << "\n";

	// Determine whether it's a file or form field based on Content-Disposition
	if (contentDisposition.find("filename=") != string::npos) {
		// It's a file upload
		string filename = extractFilename(contentDisposition);
		request.files[filename] = bodyStream.str(); // Store the file content
	} 
	else {
		// It's a form field
		string fieldName = extractFieldName(contentDisposition);
		request.formFields[fieldName] = bodyStream.str();
	}
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

string RequestParser::extractFieldName(const std::string& contentDisposition) {
	size_t namePos = contentDisposition.find("name=");
	if (namePos != std::string::npos) {
		size_t startPos = contentDisposition.find("\"", namePos + 5) + 1;
		size_t endPos = contentDisposition.find("\"", startPos);
		return contentDisposition.substr(startPos, endPos - startPos);
	}
	return "";
}