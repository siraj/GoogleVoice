// GoogleVoice API.  Created by mm_202.  http://github.com/mastermind202/GoogleVoice
#define GV_VERSION "v0.0.4"
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <boost/regex.hpp>
#include "gvoice.h"

using namespace std;
using namespace boost;

GoogleVoice::GoogleVoice()		{hcurl=NULL; loggedin=0; debug=0; version=GV_VERSION;}
GoogleVoice::~GoogleVoice()		{if(hcurl) curl_easy_cleanup(hcurl);}

int GoogleVoice::SendSMS(string number, string msg)
{
	if(!hcurl) {cout << "hcurl is NULL.  Did you forget to call Init()?\n"; return -1;}
	if(Login()) return -1;

	curl_easy_setopt(hcurl, CURLOPT_URL, "https://www.google.com/voice/sms/send/");
	curl_easy_setopt(hcurl, CURLOPT_POST, 1);
	
	string data  = "_rnr_se="+rnr_se;
	data += "&phoneNumber=1"+number;
	data += "&text="+msg;
	
	curl_easy_setopt(hcurl, CURLOPT_POSTFIELDS, data.c_str());	// TODO: check this and make sure it doesnt hold on to passed data ptr.
	curlbuf.clear(); cr=curl_easy_perform(hcurl);
	if(cr!=CURLE_OK) {cout << "curl() error on sending sms: " << errorbuf << endl; return -3;}
	
	if(debug&2) cout << "\nSendSMS curlbuf: [" << curlbuf << "]\n";

	regex rexp("\"data\":\\{\"code\":(\\d+)\\}"); cmatch m;
	if(regex_search(curlbuf.c_str(), m, rexp)) {string t=m[1]; return atoi(t.c_str());}
	else {cout << "Something went wrong.  Enable debugging.\n"; return -1;}
	
	return -1;
}

int GoogleVoice::Login(string email, string passwd)
{
	if(email.length()<1 || email.length()<1) return -1;
	this->email=email; this->passwd=passwd;
	return Login();
}

int GoogleVoice::Login(void)
{
	if(!hcurl) {cout << "hcurl is NULL.  Did you forget to call Init()?\n"; return -1;}
	if(loggedin) return 0;

	regex rexp; cmatch m;
	string post, galx;

	// Get GLAX token.
	curl_easy_setopt(hcurl, CURLOPT_URL, "https://www.google.com/accounts/ServiceLogin?passive=true&service=grandcentral");
	curlbuf.clear(); cr=curl_easy_perform(hcurl);
	if(cr!=CURLE_OK) {cout << "curl() Error: " << errorbuf << endl; return -1;}

	rexp = "name=\"GALX\"\\s*value=\"([^\"]+)\"";
	if(regex_search(curlbuf.c_str(), m, rexp)) 
	{ 
		galx=m[1];
		if(debug&1) cout << "Got GALX session token: " << galx << endl;

	} else {cout << "Failed to find GALX token.\n"; return -2;}

	// Login and get rnr_se token.
	curl_easy_setopt(hcurl, CURLOPT_URL, "https://www.google.com/accounts/ServiceLoginAuth?service=grandcentral");
	curl_easy_setopt(hcurl, CURLOPT_POST, 1);
	post  = "Email="+email;
	post += "&Passwd="+passwd;
	post += "&continue=https://www.google.com/voice/account/signin";
	post += "&service=grandcentral&GALX="+galx;
	curl_easy_setopt(hcurl, CURLOPT_POSTFIELDS, post.c_str());	// TODO: check this and make sure it doesnt hold on to passed data ptr.
	curlbuf.clear(); cr=curl_easy_perform(hcurl);
	if(cr!=CURLE_OK) {cout << "curl() Error: " << errorbuf << endl; return -3;}

	if(debug&2) cout << "\nLogin() curlbuf: [" << curlbuf << "]\n";

	rexp = "name=\"_rnr_se\".*?value=\"(.*?)\"";
	if(regex_search(curlbuf.c_str(), m, rexp)) 
	{ 
		rnr_se=m[1]; loggedin=1;
		if(debug&1) cout << "Got rnr_se session token: " << rnr_se << endl;
	}
	else
	{
		cout << "Failed to find rnr_se token. (Most likely a bad/mistyped email/passwd)\n";
		loggedin=0;
		return -2;
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

int GoogleVoice::CurlWriter(char *data, size_t size, size_t nmemb, string *buffer)
{
	if(buffer!=NULL) {buffer->append(data, size*nmemb); return size*nmemb;}
	return 0;
}

