#include <string>

#include "Client.hpp"

Client::Client(int id, std::string username) {
	this->id = id;
	this->username = username;
	fillInfo = false;
}
int Client::getId() {
	return id;
}
std::string Client::getUserName() {
	return username;
}
void Client::setUserName(std::string username) {
	this->username = username;
}
bool Client::getFillInfo() {
	return fillInfo;
}
void Client::setFillInfoTrue() {
	fillInfo = true;
}