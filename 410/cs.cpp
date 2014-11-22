#include <iostream>
#include <unistd.h>
#include <stdlib.h>

#define randomdelay usleep(rand()%20)

void cs(int threadID) {
	std::cout << "Entering critical section " << threadID << std::endl;
	randomdelay;
	std::cout << "Exiting critical section " << threadID << std::endl;
}