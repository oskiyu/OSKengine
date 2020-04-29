#include <enet/enet.h>

#include <iostream>

#include "Client.hpp"
#include "Server.hpp"


typedef int (*Suma) (int x, int y);

int a(int x, int y) {
	return x + y;
}

int main() {

	Suma xd  = &a;

	if (xd != nullptr)
		std::cout << xd(2, 3) << std::endl;


	return 0;
}