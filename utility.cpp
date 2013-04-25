#include "utility.h"


char Utility::HexToDec(char c)
{
	switch (toupper(c))
	{
	case 'A':
		return 10;
	case 'B':
		return 11;
	case 'C':
		return 12;
	case 'D':
		return 13;
	case 'E':
		return 14;
	case 'F':
		return 15;
	default:
		return c;
	}
}

char* Utility::Unescape(char* uri)
{
	char *encodedUri, *decodedUri;

	if (uri == NULL) 
	{
		return uri;
	}

	encodedUri = uri;
	decodedUri = uri;

	while (*encodedUri)
	{
		if (*encodedUri  == '%')
		{
			// flytter til Hex nummer
			encodedUri++;

			if (*encodedUri)
			{
				// konverterer hexadesima til desimal.
				*decodedUri = HexToDec(*encodedUri) * 16;
				encodedUri++;
			}

			if (*encodedUri) 
			{
				*decodedUri += HexToDec(*encodedUri);
			}
		}
		else
		{
			*decodedUri = *encodedUri;
		}

		decodedUri++;
		encodedUri++;
	}


	*decodedUri = '\0'; 

	return uri;
}

char* Utility::GetMimeType(char* path)
{
	char *szTmp;
	int i;

	szTmp = path + strlen(path) - 1;

	while ((*szTmp != '.') && (szTmp != path) && (*szTmp != '/'))
	{
		szTmp--;
	}

	szTmp++;

	if (stricmp(szTmp, "htm") == 0 || stricmp(szTmp, "html") == 0)
	{
		return "text/html";
	}
	if (stricmp(szTmp, "css") == 0)
	{
		return "text/css";
	}
	if (stricmp(szTmp, "jpeg") == 0 || stricmp(szTmp, "jpg") == 0 || stricmp(szTmp, "gif") == 0 || stricmp(szTmp, "png") == 0)
	{
		char* buf = new char[16];
		sprintf(buf, "image/%s", szTmp);
		return buf;
	}
	else 
	{
		return "text/html";
	}
}

void Utility::ReplaceChar(char* buf, char oldChar, char newChar)
{
	char* ptr;
	for(;;)
	{
		ptr = strchr(buf, oldChar);
		if(ptr == NULL) 
		{
			break; 
		}

		buf[(int)(ptr - buf)] = newChar;
	}
	return;
}


bool Utility::FileExists(const char* filename)
{
	if (FILE * file = fopen(filename, "r"))
	{
		fclose(file);
		return true;
	}
	return false;
}