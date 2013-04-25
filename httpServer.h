#ifndef _HTTPSERVER_H_
#define _HTTPSERVER_H_

#include "socket.h"
#include "httpRequest.h"


#define HTTP_PORT  80


class HttpServer
{
private:
	// server socket for listening
	Socket serverSock;

	// HTTP servernavn
	char* serverName;

public:
	HttpServer();

	/*
	* begynner HTTP server
	*/
	void Start();

	/*
	* Stop HTTP server
	*/
	void Stop();

	/*
	* Håndterer kundens ønsker
	*/
	static void ThreadWorker(void* arg);

	/*
	* Håndterer en HTTP/1.1 forespørsel. Bare forespørsel metoder GET og POST støttes
	*/
	int TreatHttp11Request(Socket* socket, HttpRequest* request);

	/*
	* Håndterer forespørsel metoder og POST ved å sende den forespurte ressursen
	*/
	void DoResource(Socket* clientSocket, HttpRequest* request);

	/*
	* Sender den generelle server header like servername osv.
	*/
	void SendServerHeader(int socket);
};

extern HttpServer server;

#endif
