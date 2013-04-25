#ifndef _SOCKET_H_
#define _SOCKET_H_

#ifdef _WIN32
#include <winsock2.h>
#endif

#define DEFAULT_BUFLEN 16384

/*
* Cross plattform socket
*/
class Socket
{
private:
	// Socket
	int _socket;

	// den sockaddr_in strukturer spesiferer address family,
	// IP address, og port.
	struct sockaddr_in _socketAddr;

	// bufferen som ble fått
	char* recvBuf;

public:
	Socket();

	Socket(sockaddr_in socketAddr);

	/*
	* initialiserer socket
	*/
	static int Initialize();

	/*
	* rydder opp socket
	*/
	static void Clean();

	/*
	* lager socket
	*/
	int Create();

	/*
	* aksepterer en innkommende tilkobling
	*/
	Socket * Accept();

	/*
	* får dataer av bytes størelse fra remote host
	*/
	int ReceiveLine();

	/*
	* sender dataer på socket
	*/
	int Send(char* buf, int socket);

	/*
	* sender en tekst eller en binær fil
	*/
	int SendFile(char* fileName, int isBinary, int socket);

	/*
	* innkommende tilkoblinger
	*/
	int Listen(short int port);

	/*
	* avslutter tilkoblingen
	*/
	int Close();

	/*
	* recvBuf
	*/
	const char* GetRecvBuf() { return recvBuf; }

	/*
	* _socket
	*/

	int GetSocket() { return _socket; }
};

#endif
