// GoogleVoice API. v0.0.5.
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
//#include <boost/regex.hpp>
#include "gvoice.h"

using namespace std;
//using namespace boost;

void PrintUsage(int verbose=0);

int main(int argc, char *argv[])
{
	GoogleVoice gv;
	int a,d,r;

	string an,am;
	string au,ap;		// These can be set as your GV user/passwd (so they dont have to be specified via CLI each time).
						// eg: au="mygmailuser"; ap="mysecretpasswd";

	while((a=getopt(argc,argv, "d::hvu:p:n:m:")) != -1)
	{
		switch(a)
		{
			case 'h':	// help
				PrintUsage(1); return 0;
			case 'v':	// version.
				printf("GoogleVoice API by mm_202.  %s (%s).  http://github.com/mastermind202/GoogleVoice\n\n", gv.GetVersion().c_str(), BUILDTS);
				return 0;
			case 'd':	// debug mode.
				if(optarg) d=atoi(optarg); else d=1;
				gv.debug=d;
				break;
			case 'u':	// username
				au=optarg;
				break;
			case 'p':	// password
				ap=optarg;
				break;
			case 'n':	// number
				an=optarg;
				break;
			case 'm':	// message
				am=optarg;
				break;
			case '?':
				PrintUsage(); return 0;
		}
	}
	if(an.empty() || am.empty() || au.empty() || ap.empty()) {PrintUsage(1); return 1;}

	printf("GoogleVoice API by mm_202.\n");

	if(gv.Init()) {cout << "GoogleVoice() failed to initialize.  Blaming curl.  Dying.\n"; return -1;}

	r = gv.Login(au,ap);
	if((gv.debug&1) || r) printf("gv.Login() returned %d.\n\n", r);

	r = gv.SendSMS(an,am);
	if((gv.debug&2) || r) printf("gv.SendSMS() returned %d.\n\n", r);
	if(!r) printf("SMS send successfully.\n");

	return r;
}

void PrintUsage(int verbose)
{
	printf("Usage: %s -hdv -u [username] -p [password] -n [number] -m [textmsg]\n", "gvapi");	//argv[0]);
	if(!verbose) return;
	printf("\t-v:\t\tGet version information.\n");
	printf("\t-d{n}:\t\tSet debug (verboseness).  If debug value {n} not specified, defaults to 1.\n\t\t\tdebug bits: 1=output GV calls. 2=dump GV call results.\n");
}


