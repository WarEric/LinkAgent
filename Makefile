ALL: linkagent

linkagent:linkagent.cc linkagent.h Logger.o
	g++ -Wall -std=c++11 -o linkagent linkagent.cc Logger.o
Logger.o:Logger.cc Logger.h
	g++ -Wall -c Logger.cc

clean:
	- rm -f *.o
	- rm -f linkagent
