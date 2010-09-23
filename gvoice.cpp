// GoogleVoice API. v0.0.1.
#include <iostream>
#include <string>
#include <curl/curl.h>
#include <stdlib.h>
#include <stdio.h>
#include <boost/regex.hpp>

using namespace std;
using namespace boost;

// http://curl.haxx.se/libcurl/
// http://adriel.dynalias.com/blog/?p=252

class GoogleVoice {
public:
	CURL *hcurl;
	CURLcode cr;

	char errorbuf[CURL_ERROR_SIZE];
	string curlbuf;
	
	string login,password;
	string galx, rnr_se;
	int loggedin;
	
	GoogleVoice()	{loggedin=0;}
	~GoogleVoice()	{if(hcurl) curl_easy_cleanup(hcurl);}
	
	int Init(void);
	int Login(string login, string password);
	int SendSMS(string number, string msg);
	
	static int CurlWriter(char *data, size_t size, size_t nmemb, string *buffer);
};

GoogleVoice gv;

int main(int argc, char *argv[])
{
	int r;

	if(gv.Init()) {printf("cURL failed to initalize! Dying.\n"); return -1;}
	
	r = gv.Login("mygmailemail", "mygmailpasswd");
	printf("gv.Login() returned %d.\n\n", r);
	
	r = gv.SendSMS("5552026202", "Hi Everyone!");
	printf("gv.SendSMS() returned %d.\n\n", r);
		
	printf("Done.\n\n");
	
	return 0;
}
int GoogleVoice::SendSMS(string number, string msg)
{
	//if(GoogleVoice::Login()) return -1;

	curl_easy_setopt(hcurl, CURLOPT_URL, "https://www.google.com/voice/sms/send/");
	curl_easy_setopt(hcurl, CURLOPT_POST, 1);
	
	string data  = "_rnr_se="+rnr_se;
	data += "&phoneNumber=1"+number;
	data += "&text="+msg;
	
	curl_easy_setopt(hcurl, CURLOPT_POSTFIELDS, data.c_str());	// TODO: check this and make sure it doesnt hold on to passed data ptr.
	curlbuf.clear(); cr=curl_easy_perform(hcurl);
	if(cr!=CURLE_OK) {cout << "curl() error on sending sms: " << errorbuf << endl; return -3;}
	
	cout << "\n\nSendSMS raw result: [" << curlbuf << "]\n\n";

	return 0;
}
int GoogleVoice::CurlWriter(char *data, size_t size, size_t nmemb, string *buffer)
{
	if(buffer!=NULL)
	{
		buffer->append(data, size*nmemb);
		return size*nmemb;
	}
	return 0;
}

int GoogleVoice::Init(void)
{
	hcurl=curl_easy_init(); if(!hcurl) return -1;
	curl_easy_setopt(hcurl, CURLOPT_ERRORBUFFER, errorbuf);
	curl_easy_setopt(hcurl, CURLOPT_WRITEFUNCTION, CurlWriter);
	curl_easy_setopt(hcurl, CURLOPT_WRITEDATA, &curlbuf);

	curl_easy_setopt(hcurl, CURLOPT_HEADER, 0);
	curl_easy_setopt(hcurl, CURLOPT_FOLLOWLOCATION, 1);
	curl_easy_setopt(hcurl, CURLOPT_USERAGENT, "Mozilla/5.0 (X11; U; Linux x86_64; en-US; rv:1.9.2.8) Gecko/20100804 Gentoo Firefox/3.6.8");

	curl_easy_setopt(hcurl, CURLOPT_COOKIEJAR, "cookie.txt");
	return 0;
}

int GoogleVoice::Login(string login, string passwd)
{
	if(loggedin) return 0;

	regex rexp; cmatch m;
	string data;
	int i;

	// Get GLAX token.
	curl_easy_setopt(hcurl, CURLOPT_URL, "https://www.google.com/accounts/ServiceLogin?passive=true&service=grandcentral");
	curlbuf.clear(); cr=curl_easy_perform(hcurl);
	if(cr!=CURLE_OK) {cout << "curl() Error: " << errorbuf << endl; return -1;}

	rexp = "name=\"GALX\"\\s*value=\"([^\"]+)\"";
	if(regex_search(curlbuf.c_str(), m, rexp)) 
	{ 
		if(m.size()==2) galx=m[1];
		cout << "Got GALX session token: " << galx << endl;
		/*if(m.size()>0) cout << "m[0].f=[" << m[0] << "]\n";
		if(m.size()>1) cout << "m[1].f=[" << m[1] << "]\n";
		if(m.size()>2) cout << "m[2].f=[" << m[2] << "]\n";*/
	} else {cout << "Failed to find GALX token.\n"; return -2;}

	// Login and get rnr_se token.
	curl_easy_setopt(hcurl, CURLOPT_URL, "https://www.google.com/accounts/ServiceLoginAuth?service=grandcentral");
	curl_easy_setopt(hcurl, CURLOPT_POST, 1);
	data  = "Email="+login;
	data += "&Passwd="+passwd;
	data += "&continue=https://www.google.com/voice/account/signin";
	data += "&service=grandcentral";
	data += "&GALX="+galx;
	curl_easy_setopt(hcurl, CURLOPT_POSTFIELDS, data.c_str());	// TODO: check this and make sure it doesnt hold on to passed data ptr.
	curlbuf.clear(); cr=curl_easy_perform(hcurl);
	if(cr!=CURLE_OK) {cout << "curl() Error: " << errorbuf << endl; return -3;}

	rexp = "name=\"_rnr_se\".*?value=\"(.*?)\"";
	if(regex_search(curlbuf.c_str(), m, rexp)) 
	{ 
		if(m.size()==2) {rnr_se=m[1]; loggedin=1;}
		cout << "Got rnr_se session token: " << rnr_se << endl;
	}
	else
	{
		cout << "Failed to find rnr_se token  (login failure).\n";
		loggedin=0;
		return -2;
	}

	return 0;
}



