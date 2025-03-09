#include <iostream>
#include <sys/time.h>
#include <sstream>

std::string getTime() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    long time = tv.tv_sec * 1000 + tv.tv_usec / 1000;

	std::stringstream stream;
	stream << time;
	return stream.str();
}

int main() {
	std::string body = "<h2>Current Unix Time</h2><p>" + getTime() + "</p>";
	ssize_t content_length = body.size();

	std::cout << "HTTP/1.1 200 OK" << std::endl;
	std::cout << "Content-Type: text/html" << std::endl;
	std::cout << "Content-Length: " << content_length << std::endl;
	std::cout << "Connection: close\n" << std::endl;

	std::cout << body << std::endl;

	return 0;
}