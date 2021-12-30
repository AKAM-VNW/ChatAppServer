#include <iostream>
#include "Server.hpp"

int main() {
	Server s1("Void", "127.0.0.1", 54000);
	s1.startChatService();
	system("pause");
}