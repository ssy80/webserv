#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "../header/ReqServer.hpp"

#define PORT 8080

ReqServer::~ReqServer()
{}

void handle_client(int client_socket) {
	char buffer[1024];
	
	// Read the request from the client (Telnet)
	int bytes_received = read(client_socket, buffer, sizeof(buffer) - 1);
	if (bytes_received < 0) {
			std::cerr << "Error reading from socket" << endl;
			return;
	}
	buffer[bytes_received] = '\0';  // Null-terminate the buffer to make it a valid string
	Request req = RequestParser::parseRequest(buffer);
	
	cout << "Received request: " << endl;
	req.print();
	// cout << buffer << endl;

	// Send an HTTP response
	const char* response = 
			"HTTP/1.1 200 OK\r\n"
			"Content-Type: text/plain\r\n"
			"Connection: close\r\n"
			"\r\n"
			"Hello, Telnet client!\r\n";

	int bytes_sent = send(client_socket, response, strlen(response), 0);
	if (bytes_sent < 0) {
			std::cerr << "Error sending response" << endl;
			return;
	}

	// Close the client connection
	close(client_socket);
}

void ReqServer::startServer(){
	int server_socket, client_socket;
	struct sockaddr_in server_addr, client_addr;
	socklen_t client_len = sizeof(client_addr);

	// Create the server socket
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (server_socket < 0) {
			std::cerr << "Error opening socket" << endl;
			exit(1);
	}

	// Set up the server address structure
	bzero((char*)&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;  // Accept connections from any IP
	server_addr.sin_port = htons(PORT);

	// Bind the socket to the address
	if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
			std::cerr << "Error binding socket" << endl;
			exit(1);
	}

	// Start listening for incoming connections
	if (listen(server_socket, 5) < 0) {
			std::cerr << "Error listening on socket" << endl;
			exit(1);
	}

	std::cout << "Server is running on port " << PORT << ". Waiting for connections..." << endl;

	while (true) {
			// Accept a client connection
			client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
			if (client_socket < 0) {
					std::cerr << "Error accepting client connection" << endl;
					continue;
			}

			// Handle the client request
			handle_client(client_socket);
	}

	// Close the server socket
	close(server_socket);
}



