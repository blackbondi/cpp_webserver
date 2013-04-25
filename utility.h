#ifndef _UTILITY_H_
#define _UTILITY_H_

#include <iostream>
#include <sys/stat.h>


/*
* denne klassen innholder nyttige funksjoner
*/
class Utility
{
public:
	/*
	* konverterer en heks tegn til desimal tegn.
	*/
	static char HexToDec(char c);

	/*
	* Unescapes escape characters
	* kilder: https://en.wikipedia.org/wiki/Percent-encoding
	*/
	static char* Utility::Unescape(char *uri);

	/*
	* Retrieves the MIME-Type of 
	* kilder: https://en.wikipedia.org/wiki/MIME
	*/
	static char* GetMimeType(char* path);

	/*
	* erstatter et tegn til et annet string tegn
	*/
	static void ReplaceChar(char* buf, char oldChar, char newChar);

	/*
	* sjekker hvis en fil ikke eksisterer
	*/
	static bool Utility::FileExists(const char* filename);
};

#endif