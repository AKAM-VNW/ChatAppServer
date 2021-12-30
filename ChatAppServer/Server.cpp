#include <string>

#include "Server.hpp"

Server::Server(std::string name, std::string ipAddr, int port) {
	//Initialise winsock
	if (WSAStartup(MAKEWORD(2, 2), &wsData)) {
		std::cerr << "Can't initialise winsock" << std::endl;
	}
	else {
		//Create socket
		listenSocket = socket(AF_INET, SOCK_STREAM, 0);
		if (listenSocket == INVALID_SOCKET) {
			std::cerr << "Can't create socket" << std::endl;
		}
		else {
			//Init server vars
			_name = name;
			_ipAddr = ipAddr;
			_port = port;
			//Create server socket address
			sockaddr_in serverAddr;
			serverAddr.sin_family = AF_INET;
			serverAddr.sin_port = htons(port);
			serverAddr.sin_addr.S_un.S_addr = INADDR_ANY;

			//Bind socket to ip and port
			bind(listenSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));

			//Tell winsock that listenSocket is for listening
			listen(listenSocket, SOMAXCONN);

			FD_ZERO(&socketSet);

			FD_SET(listenSocket, &socketSet);
		}
	}
}
Server::~Server() {}
void Server::startChatService() {
	while (true) {
		//Need a copy of fd_set because select does destroy the fd_set passed in param
		fd_set socketSetCpy = socketSet;

		int socketCount = select(0, &socketSetCpy, nullptr, nullptr, nullptr);

		for (int i = 0; i < socketCount; i++) {
			//Get socket the we will work on
			SOCKET socket = socketSetCpy.fd_array[i];

			if (socket == listenSocket) {
				//Accept incoming connection
				SOCKET newUser = accept(listenSocket, nullptr, nullptr);

				//Add new client socket to fd_set
				FD_SET(newUser, &socketSet);
				clients.push_back(Client(newUser));

				std::cout << "New socket =>" + std::to_string(newUser) + "<= is in" << std::endl;

				//Send welcome to client
				std::string msg = "Welcome to " + _name + " chat server! \r\nServer: send your username\r\n";
				send(newUser, msg.c_str(), msg.length(), 0);
			}
			else {
				std::string msg = "";
				char buf[BUFSIZE];
				ZeroMemory(buf, BUFSIZE);

				//Receive the msg
				int bytesRecv = recv(socket, buf, BUFSIZE, 0);

				if (bytesRecv <= 0) {
					//Clear client socket
					closesocket(socket);
					FD_CLR(socket, &socketSet);
					for (int j = 0; j < clients.size(); j++) {
						if (clients[j].getId() == socket) {
							std::cout << "Socket " + std::to_string(socket) + " | " + clients[j].getUserName() + " is out" << std::endl;

							//Inform clients that the user is disconnected
							msg = "Server: " + std::to_string(clients[j].getId()) + " | " + clients[j].getUserName() + " has disconnected\r\n";
							for (int j = 0; j < socketSet.fd_count; j++) {
								SOCKET toClient = socketSet.fd_array[j];
								if (toClient != listenSocket && toClient != socket) {
									send(toClient, msg.c_str(), msg.length(), 0);
								}
							}
							msg.clear();
							clients.erase(clients.begin() + j);
							break;
						}
					}
				}
				else {
					//Set Client username after getting it
					std::string bufStr(buf); //It could be the username or the msg
					bufStr.erase(std::remove(bufStr.begin(), bufStr.end(), '\r'), bufStr.end());
					bufStr.erase(std::remove(bufStr.begin(), bufStr.end(), '\n'), bufStr.end());
					if (bufStr.length() == 0) {
						break;
					}

					bool gotClientUserName = false;
					int clientIndex = -1;

					for (int j = 0; j < clients.size(); j++) {
						if (clients[j].getId() == socket) {
							clientIndex = j;
							if (!clients[j].getFillInfo()) {
								clients[j].setUserName(bufStr);
								clients[j].setFillInfoTrue();
								gotClientUserName = true;
								std::cout << "Socket " + std::to_string(socket) + " =>" + bufStr + "<=" << std::endl;
								break;
							}
						}
					}

					//Inform other clients
					if (gotClientUserName) {
						msg = "Server: New user " + std::to_string(clients[clientIndex].getId()) + " / " + clients[clientIndex].getUserName() + " has connected\r\n";
						for (int j = 0; j < socketSet.fd_count; j++) {
							SOCKET toClient = socketSet.fd_array[j];
							if (toClient != listenSocket && toClient != socket && clients[j - 1].getFillInfo()) {
								send(toClient, msg.c_str(), msg.length(), 0);
							}
						}
						msg.clear();
					}
					//Send msg to other clients
					else {
						msg = std::to_string(clients[clientIndex].getId()) + " | " + clients[clientIndex].getUserName() + ": " + bufStr + "\r\n";
						std::cout << "Socket " + std::to_string(socket) + " | " + clients[clientIndex].getUserName() + " =>" + bufStr + "<=" << std::endl;
						for (int j = 0; j < socketSet.fd_count; j++) {
							SOCKET toClient = socketSet.fd_array[j];
							if (toClient != listenSocket && toClient != socket && clients[j - 1].getFillInfo()) {
								send(toClient, msg.c_str(), msg.length(), 0);
							}
						}
						msg.clear();
					}
				}
			}
		}
	}
	closesocket(listenSocket);
	WSACleanup();
}