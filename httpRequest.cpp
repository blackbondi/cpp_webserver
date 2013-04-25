#include <iostream>
#include "socket.h"
#include "httpRequest.h"

using namespace std;

HttpRequest::HttpRequest(Socket* clientSocket)
{
	requestMethod = new char[8];
	requestedUri = new char[8192];
	requestedUriParameter = new char[8192];
	httpVersion = new char[9];

	/* 
	* Forste linje for å bestemme foresporselsmetoden, 
	*  Uri og  HTTP standard version
	* Denne linjen skal se slik ut: 
	* RequestMethod URI Version 
	* e. g. 
	* GET /infotext.html HTTP/1.1
	*/
	char* tmpBuf = new char[8192];
	sscanf(clientSocket->GetRecvBuf(), "%s %s %s", requestMethod, tmpBuf, httpVersion);

	// Sjekk om den forespurte Uri inneholder en spørring. 
	//Spørringen starter med spørsmålstegn?
	memset(requestedUriParameter, 0, sizeof(requestedUriParameter));
	char* separator = strchr(tmpBuf, '?');
	if (separator != NULL)
	{
		*separator = NULL;
		separator++;
		requestedUriParameter = strdup(separator);
	}
	requestedUri = tmpBuf;

	// Receive the other
	ReceiveHeaders(clientSocket);
}

HttpRequest::~HttpRequest()
{
	delete requestMethod;
	delete requestedUri;
	delete requestedUriParameter;
	delete httpVersion;
}

HttpVersion HttpRequest::GetHttpVersion()
{
	if (_stricmp(httpVersion, "HTTP/0.9") == 0)
	{
		return HttpVersion_09;
	}
	else if (_stricmp(httpVersion, "HTTP/1.0") == 0)
	{
		return HttpVersion_10;
	}
	else if (_stricmp(httpVersion, "HTTP/1.1") == 0)
	{
		return HttpVersion_11;
	}
	else 
	{
		return HttpVersion_Unknown;
	}
}

HttpRequestMethods HttpRequest::GetHttpRequestMethod()
{
	if(_stricmp(requestMethod, "GET") == 0)
	{
		return GET;
	}
	else if(_stricmp(requestMethod, "POST") == 0)
	{
		return POST;
	}
	else
	{
		return UNKNOWN;
	}
}

char* HttpRequest::GetRequestedUri()
{
	return requestedUri;
}

char* HttpRequest::GetRequestedUriParameter()
{
	return requestedUriParameter;
}

void HttpRequest::ReceiveHeaders(Socket* clientSocket)
{
	int retCode;
	char* recvBuf = new char[HTTP_BUF];
	recvBuf = (char*)clientSocket->GetRecvBuf();

	do
	{
		retCode = clientSocket->ReceiveLine();
	}
	while(retCode > 0 && 
		// The end of the header fields is indicated by an empty field, 
			// resulting in the transmission of two consecutive CR-LF pairs
				_stricmp(recvBuf, "\r\n") != 0);
}
