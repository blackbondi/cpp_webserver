#include <fstream>
#include <iostream>
#include "socket.h"

using namespace std;

Socket::Socket()
{
}

Socket::Socket(sockaddr_in socketAddr)
{
	_socket = -1; // INVALID_SOCKET
	recvBuf = new char[DEFAULT_BUFLEN];
	_socketAddr = socketAddr;
}

int Socket::Initialize() 
{
	int retCode;

	// initialiser Winsock
#ifdef _WIN32
	// den høyeste versjon av windows sockets spesifikasjon som blir brukt.
	WORD wVersionRequested; 
	WSADATA wsaData; 
	wVersionRequested = MAKEWORD(2, 2); 

	// WSAStartup funksjonen initialiserer bruken av Winsock DLL.
	retCode = WSAStartup(wVersionRequested, &wsaData); 

	// hvis det lykkes, WSAStartup funksjonen returnerer null
	if (retCode != 0) 
	{
		retCode = WSAGetLastError();
	}
#endif

	return retCode;
}

void Socket::Clean()
{
#ifdef _WIN32
	WSACleanup();
#endif
}

int Socket::Create()
{
	// lager en socket med internet protocol version 4(IPv4) som en adress family (AF_INET)
	// sekvensert, pålitelig, to-veis, connection-based byte streamer som en type spekifikasjon (SOCK_STREAM)
	// Transmission Control Protocol (TCP) som protocol
	_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

#ifdef _WIN32
	if (_socket == INVALID_SOCKET)
	{
		return WSAGetLastError();
	}
#endif

	return _socket;
}

int Socket::ReceiveLine()
{
	int retCode;

#ifdef _WIN32
	fd_set nfds;
	struct timeval timeout;
#endif

	memset(recvBuf, 0, DEFAULT_BUFLEN);

	// sjekker ut socket for lesbarhet før hente dataene
#ifdef _WIN32
	FD_ZERO(&nfds);
	FD_SET(_socket, &nfds);

	// den maksimale tiden til å vente er satt på 1 minutt
	timeout.tv_sec = 60;

	// Første parameteren er ignorert
	retCode = select(1, &nfds, 0, 0, &timeout);

	if (retCode == SOCKET_ERROR)
	{
		return WSAGetLastError();
	}
	else if(retCode < 1)
	{
		// Enten kontakten ikke er klar eller tidfristen er utløpt
		return -1;
	}
#endif

	int index = 0;
	int bytes = 1;

	do
	{
		retCode = recv(_socket, recvBuf + index, bytes, 0);
	}
	while(retCode > 0 && recvBuf != NULL && recvBuf[index++] != '\n'); //Hver linje avsluttes med en carriage return(CR) og line feed (LF) tegnet sekvens

	recvBuf[index] = '\0';

#ifdef _WIN32
	if (retCode == SOCKET_ERROR)
	{
		cerr << "Receiving data failed with error: " << WSAGetLastError() << endl;
	}
#endif

	// tilkoblingen er stengt :)
	if(retCode == 0)
	{
		cerr << "dataene kan ikke hentes for tilkoblingen har blitt stengt." << endl;
	}

	return retCode;
}

Socket* Socket::Accept()
{
	struct sockaddr_in sockAddr;
	memset((void*)&sockAddr, 0, sizeof(sockAddr));

	int sock = -1;
	int addrLen = sizeof(sockAddr);

	sock = accept(_socket, (struct sockaddr*)&(sockAddr), &addrLen);

	if (sock < 0)
	{
		return NULL;
	}
	else
	{
		Socket* socket = new Socket(sockAddr);
		socket->_socket = sock;

		return socket;
	}
}

int Socket::Listen(short int port)
{
	int retCode;

	memset((void *)&_socketAddr, 0, sizeof(_socketAddr));

	_socketAddr.sin_family = AF_INET;
	_socketAddr.sin_port = htons(port);
	_socketAddr.sin_addr.s_addr = INADDR_ANY;

	//binde seg til den gitte porten.
	retCode = bind(_socket, (struct sockaddr*) &_socketAddr, sizeof(_socketAddr));
	if (retCode < 0)
	{
#ifdef _WIN32
		if (retCode == SOCKET_ERROR)
		{
			retCode = WSAGetLastError();
		}
#endif
		return retCode;
	}

	// den lyttes til de innkommende tilkoblinger
	retCode = listen(_socket, SOMAXCONN);
	if (retCode < 0)
	{
		return retCode;
	}

	return 0;
}

int Socket::Close()
{
	int retCode = -1;

	memset(recvBuf, 0, DEFAULT_BUFLEN);

#ifdef _WIN32
	retCode = closesocket(_socket);

	if (retCode == SOCKET_ERROR)
	{
		return WSAGetLastError();
	}
#endif

	// Hvis det ikke skjer noe feil, closesocket returnerer null
	return retCode;
}

int Socket::Send(char* buf, int socket)
{
	int retCode;

	retCode = send(socket, buf, strlen(buf), 0);

#ifdef _WIN32
	if(retCode == SOCKET_ERROR)
	{
		return WSAGetLastError();
	}
#endif

	return retCode;
}

int Socket::SendFile(char* fileName, int isBinary, int socket)
{
	ifstream fileStream;
	int retCode = 0;
	char* lineBuf = new char[DEFAULT_BUFLEN];

	if(isBinary == TRUE)
	{
		fileStream.open(fileName, ios::binary);
	}
	else
	{
		fileStream.open(fileName);
	}

	if (!fileStream)
	{
		return -1;
	}

	if(isBinary == TRUE)
	{
		while ( ! fileStream.eof() )
		{
			fileStream.read(lineBuf, DEFAULT_BUFLEN);
			retCode += send(socket, lineBuf, fileStream.gcount(), 0);
		}
	}
	else
	{
		do
		{
			memset(lineBuf, 0, DEFAULT_BUFLEN);
			fileStream.getline(lineBuf, DEFAULT_BUFLEN, '\n');
			retCode += send(socket, lineBuf, strlen(lineBuf), 0);
			retCode += send(socket, "\r\n", strlen("\r\n"), 0);
		}
		while (!fileStream.eof());

		// sender den siste linje uten eol (end of line)
		memset(lineBuf, 0, DEFAULT_BUFLEN);
		fileStream.getline(lineBuf, DEFAULT_BUFLEN, '\n');
		retCode += send(socket, lineBuf, strlen(lineBuf), 0);
	}

	fileStream.close();
	delete [] lineBuf;

	return retCode;
}
