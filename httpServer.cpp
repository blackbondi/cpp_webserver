#include <iostream>
#include <process.h>
#include <sys/stat.h>
#include "utility.h"
#include "httpServer.h"

using namespace std;

HttpServer::HttpServer() 
{
	serverName = new char[16];
	serverName = "MyServer/1.0"; 
}

void HttpServer::Start()
{
	int retCode;

	// The client socket
	Socket* clientSock;

	cout << "Starting HTTP Server." << endl << endl;

	retCode = Socket::Initialize();

	if(retCode != 0)
	{
		cerr << "Initializing the socket failed with error: " << retCode << endl;
		return;
	}

	if (serverSock.Create() < 0)
	{
		cerr << "Creating the socket failed with error: " << retCode << endl;
		return;
	}

	// Begin the listening
	retCode = serverSock.Listen(HTTP_PORT);

	if(retCode != 0)
	{
		cerr << "Starting the listening failed with error: " << retCode << endl;
		return;
	}

	do
	{
		// Aksepter innkommende tilkoblinger
		clientSock = serverSock.Accept();

		if (clientSock != NULL)
		{
			retCode = _beginthread(&HttpServer::ThreadWorker, 50000, (void*) clientSock);

			// _beginthread returns –1 on an error
			if (retCode == -1)
			{
				clientSock->Close();
				delete clientSock;
			}
		}
	}
	while(true);
}

void HttpServer::Stop()
{
	cout << "Stopping HTTP Server." << endl;
	serverSock.Close();
	Socket::Clean();
}

void HttpServer::ThreadWorker(void* arg)
{
	int retCode;
	Socket* clientSocket = (Socket*) arg;


	// Motta den første linjen for forespørselen
	retCode = clientSocket->ReceiveLine();

	if(retCode > 0)
	{
		cout << "The following request was received:" << endl;
		cout << clientSocket->GetRecvBuf() << endl;

		HttpRequest* request = new HttpRequest(clientSocket);

		// Versjon spesifikk behandling
		switch (request->GetHttpVersion())
		{
		case HttpVersion_09:
			break;

		case HttpVersion_10:
			break;

		case HttpVersion_11:
			{
				server.TreatHttp11Request(clientSocket, request);
				break;
			}

		default:
			cerr << "Unknown HTTP version " << request->GetHttpVersion() << endl;
			break;
		}

		delete request;
	}

	clientSocket->Close();
	delete clientSocket;
}

int HttpServer::TreatHttp11Request(Socket* clientSocket, HttpRequest* request)
{
	switch (request->GetHttpRequestMethod())
	{
	case GET:
		DoResource(clientSocket, request);
		return TRUE;

	case POST:
		DoResource(clientSocket, request);
		return TRUE;

	default:
		cerr << "Unknown or unsupported HTTP method " << request->GetHttpRequestMethod() << endl;
		return FALSE;
	}

}

void HttpServer::DoResource(Socket* clientSocket, HttpRequest* request)
{
	serverSock.Send("HTTP/1.1 200 OK\r\n", clientSocket->GetSocket());
	SendServerHeader(clientSocket->GetSocket());

	char* uncPath = request->GetRequestedUri();
	char* path = new char[8192];

	Utility::Unescape(path);

	if (_stricmp(uncPath, "/") == 0)
	{
		// Send velkommen fil.
		path = ".\\htdocs\\index.html";
	}
	else 
	{
		// konvertere UNC path til lokal path
		Utility::ReplaceChar(uncPath, '/', '\\');
		sprintf(path, ".\\htdocs%s", uncPath);
	}

	if(Utility::FileExists(path))
	{
		char* buf = new char[128];

		// få MIME-Type
		char* mimeType = Utility::GetMimeType(path);

		sprintf(buf, "Content-Type: %s\r\n", mimeType);
		serverSock.Send(buf, clientSocket->GetSocket());

		// Få filstørrelse
		struct stat st;
		stat(path, &st);

		sprintf(buf, "Content-Length: %d\r\n", st.st_size);
		serverSock.Send(buf, clientSocket->GetSocket());
		serverSock.Send("\r\n", clientSocket->GetSocket());

		// Binary or text file?
		int isBinary;
		if (strstr(mimeType, "text/") == NULL)
		{
			isBinary = TRUE;
		}
		else
		{
			isBinary = FALSE;
		}

		// Send filen
		serverSock.SendFile(path, isBinary, clientSocket->GetSocket());
	}
	else
	{
		serverSock.Send("HTTP/1.1 404 Not Found\r\n", clientSocket->GetSocket());
		SendServerHeader(clientSocket->GetSocket());
		serverSock.Send("\r\n", clientSocket->GetSocket());
	}
}

void HttpServer::SendServerHeader(int socket)
{
	char* buf = new char[32];
	sprintf(buf, "Server: %s\r\n", serverName);
	serverSock.Send(buf, socket);
}