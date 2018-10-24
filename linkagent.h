/**
 * Define some data class for linkagent.
 * by wareric@163.com 2018-10-22
**/

#ifndef LINKAGENT_H_H
#define LINKAGENT_H_H

#include <string>
#include <queue>
#include <unistd.h>

class Client{
	public:
		Client(int fd):socketfd(fd){}

		int socketfd;
		std::queue<std::string> reply;
};

class LinkTask{
	public:
		enum TYPE{
			BANDWIDTH,
			DELAY,
		};

		LinkTask(int fd, std::string dst, std::string c):type(DELAY), sockfd(fd),
				 program("ping"), destination(dst), count(c){}
		LinkTask(int fd, std::string dst, std::string band, std::string t):type(BANDWIDTH), sockfd(fd),
				 program("iperf"), protocol("-u"), destination(dst), bandwith(band), time(t){}

		int type;
		int sockfd;
		std::string program;
		std::string protocol;
		std::string destination;
		std::string count;
		std::string bandwith;
		std::string time;
};
#endif
