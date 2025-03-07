#include <iostream>
#include <sys/time.h>

long getTime() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (long)(tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

int main() {
	std::cout << "<h2>Current Unix Time</h2>";
	std::cout << getTime() << std::endl;
	return 0;
}