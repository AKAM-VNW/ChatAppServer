#pragma once
#include <iostream>
#include <vector>
#include <WS2tcpip.h>
#include "Client.hpp"

#pragma comment (lib, "ws2_32.lib")

#define BUFSIZE 4096


class Server
{
	WSADATA wsData;
	SOCKET listenSocket;
	fd_set socketSet;
	std::vector<Client> clients;
	//
	std::string _name;
	std::string _ipAddr;
	int _port;

public:
	Server(std::string _name, std::string ipAddr, int port);
	~Server();
	void startChatService();
};