ALL: linkagent

linkagent:linkagent.cc linkagent.h linktask.h simplecode.o reply.o config.o Logger.o
	g++ -Wall -std=c++11 -o linkagent linkagent.cc simplecode.o reply.o config.o Logger.o
simplecode.o:simplecode.cc simplecode.h code.h
	g++ -Wall -std=c++11 -c simplecode.cc
reply.o:reply.cc reply.h
	g++ -Wall -std=c++11 -c reply.cc
config.o:config.cc config.h
	g++ -Wall -std=c++11 -c config.cc
Logger.o:Logger.cc Logger.h
	g++ -Wall -std=c++11 -c Logger.cc

clean:
	- rm -f *.o
	- rm -f linkagent
