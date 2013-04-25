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
	* H�ndterer kundens �nsker
	*/
	static void ThreadWorker(void* arg);

	/*
	* H�ndterer en HTTP/1.1 foresp�rsel. Bare foresp�rsel metoder GET og POST st�ttes
	*/
	int TreatHttp11Request(Socket* socket, HttpRequest* request);

	/*
	* H�ndterer foresp�rsel metoder og POST ved � sende den forespurte ressursen
	*/
	void DoResource(Socket* clientSocket, HttpRequest* request);

	/*
	* Sender den generelle server header like servername osv.
	*/
	void SendServerHeader(int socket);
};

extern HttpServer server;

#endif
