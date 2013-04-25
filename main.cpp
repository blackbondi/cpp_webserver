#include <cstdlib>
#include <iostream>
#include <signal.h>
#include "httpServer.h"

using namespace std;

HttpServer server;

int main(int argc, char* argv[])
{
	server.Start();

	getchar();

	return EXIT_SUCCESS;
}