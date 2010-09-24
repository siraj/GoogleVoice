#ifndef __GVOICE_H__
#define __GVOICE_H__

#include <string>
#include <curl/curl.h>

using namespace std;

class GoogleVoice {
	CURL *hcurl;
	CURLcode cr;
	string email,passwd;
	string rnr_se;			// Session token that Google needs.  NFI on what it stands for.
protected:
	string version;
public:
	char errorbuf[CURL_ERROR_SIZE];
	string curlbuf;
	
	int loggedin;
	int debug;				// Temp flag used to mostly dump the contents of curlbuf.
	GoogleVoice();
	~GoogleVoice();
	string GetVersion(void)	const {return version;}		// TODO: Later possibly return a.b.c as integers?

	int Init(void);
	int Login(void);
	int Login(string login, string passwd);
	//int Logout(void);
	
	int SendSMS(string number, string msg);
	
	static int CurlWriter(char *data, size_t size, size_t nmemb, string *buffer);
};

#endif
