#pragma once
class Client
{
	int id;
	std::string username;
	bool fillInfo;

public:
	Client(int, std::string = "");
	int getId();
	std::string getUserName();
	void setUserName(std::string);
	bool getFillInfo();
	void setFillInfoTrue();
};