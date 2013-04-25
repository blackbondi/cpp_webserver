#ifndef _HTTPREQUEST_H_
#define _HTTPREQUEST_H_

#define HTTP_BUF   8192 

// De tre standarder for HTTP er 0.9, 1.0 and 1.1
// s. http://en.wikipedia.org/wiki/Hypertext_Transfer_Protocol#History
enum HttpVersion
{
	HttpVersion_09,	// HTTP/0.9
	HttpVersion_10,	// HTTP/1.0
	HttpVersion_11,	// HTTP/1.1
	HttpVersion_Unknown
};

// HTTP request methods
// HTTP servers er nødvendig for å gjennomføre i det minste GET og HEAD metoder og, 
// når det er mulig, også OPTIONS metode.
// s. http://en.wikipedia.org/wiki/Hypertext_Transfer_Protocol#Request_methods
enum HttpRequestMethods
{ 
	GET,
	POST,
	UNKNOWN
};

class HttpRequest
{
private:
	char* requestMethod;
	char* requestedUri;
	char* requestedUriParameter;
	char* httpVersion;


public:
	HttpRequest(Socket* clientSocket);
	~HttpRequest();

	HttpVersion GetHttpVersion();
	HttpRequestMethods GetHttpRequestMethod();
	char* GetRequestedUri();
	char* GetRequestedUriParameter();

	/*
	* s. http://en.wikipedia.org/wiki/List_of_HTTP_header_fields
	*/
	void ReceiveHeaders(Socket* clientSocket);
};

#endif