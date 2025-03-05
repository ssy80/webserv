#include "../header/YSServer.hpp"
#include "../header/Response.hpp"
#include "../header/MIME.hpp"
#include "../header/ResHelper.hpp"
#include "../header/ConfigLocation.hpp"

YSServer::~YSServer()
{}

static void sendRes(int client_socket, const string& output){
	int bytes_sent = send(client_socket, output.c_str(), output.size(), 0);
	if (bytes_sent < 0) {
			std::cerr << "Error sending response" << endl;
			return;
	}
}

static void handle_client(int client_socket, string dir) {
	if (dir.empty())
		return;
	char buffer[500000];
	
	// Read the request from the client (Telnet)
	int bytes_received = read(client_socket, buffer, sizeof(buffer) - 1);
	if (bytes_received < 0) {
			std::cerr << "Error reading from socket" << endl;
			return;
	}
	buffer[bytes_received] = '\0';  // Null-terminate the buffer to make it a valid string
	
	std::cout << "~~~ BUFFER: " << buffer << std::endl;
    Request req = RequestParser::parseRequest(buffer);
    // std::cout << "Method: " << req.method << std::endl;
    // std::cout << "URL: " << req.url << std::endl;
	std::cout << "~~~ REQ ~~~ " << std::endl;
	req.print();
	
	ConfigLocation configLocation;
	configLocation.populate("on", "index.html", "POST GET DELETE", "", "/", "./www");

	std::string resp;

    if (req.method == "GET") {
		std::cout << "entering get handler" << std::endl;      
		resp = getHandler(req, configLocation);
	} else if (req.method == "POST") {
		std::cout << "entering post handler" << std::endl;      
		resp = postHandler(req, configLocation);
	} else if (req.method == "DELETE") {
		std::cout << "entering delete handler" << std::endl;      
		resp = deleteHandler(req, configLocation);
	} else {   
		std::cout << "entering other handler" << std::endl;
		resp = otherHandler();
	}

	sendRes(client_socket, resp);
	close(client_socket);
}

void YSServer::startServer(){
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
	server_addr.sin_port = htons(PORTYS);

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

	std::cout << "Server is running on port " << PORTYS << ". Waiting for connections..." << endl;

	while (true) {
			// Accept a client connection
			client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
			if (client_socket < 0) {
					std::cerr << "Error accepting client connection" << endl;
					continue;
			}
			// Handle the client request
			handle_client(client_socket, "./www");
	}

	// Close the server socket
	close(server_socket);
}