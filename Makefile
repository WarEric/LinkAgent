ALL: linkagent

linkagent:linkagent.cc linkagent.h linktask.h simplecode.o reply.o config.o logger.o
	g++ -Wall -std=c++11 -o linkagent linkagent.cc simplecode.o reply.o config.o logger.o
simplecode.o:simplecode.cc simplecode.h code.h
	g++ -Wall -std=c++11 -c simplecode.cc
reply.o:reply.cc reply.h
	g++ -Wall -std=c++11 -c reply.cc
config.o:config.cc config.h
	g++ -Wall -std=c++11 -c config.cc
logger.o:logger.cc logger.h
	g++ -Wall -std=c++11 -c logger.cc -lpthread

clean:
	- rm -f *.o
	- rm -f linkagent
