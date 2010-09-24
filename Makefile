#GITREV  = -D'GITREV="$(shell git log -1 --pretty=format:"%h by %an on %ai")"'
BLDDEF  = -D'BUILDTS="$(shell date +"%y%m%d %H:%M:%S %z")"'

gvapi: gvapi.cpp gvoice.cpp gvoice.h gvoice
	g++ -Wall $(BLDDEF) -g -lcurl -lboost_regex -o gvapi gvoice.o gvapi.cpp

gvoice: gvoice.cpp gvoice.h
	g++ -Wall $(BLDDEF) -g -c -lcurl -lboost_regex gvoice.cpp

clean:
	rm *.o gvapi 2>/dev/null

