/**
 * An basic linktask. You can treat it as a dedicated type by it's type flag.
 *
 * by wareric@163.com
 * 2018-11-07
 **/
#ifndef LINKTASK_H_H
#define LINKTASK_H_H
#include <string>

class LinkTask{
	public:
		enum TYPE{
			BANDWIDTH,
			DELAY,
			NEXTHOP,
			PEER,
			ERROR,
		};

		LinkTask():type(ERROR){}
		
		//This is a temporary function for experiment
		LinkTask(int fd){type = PEER, sockfd = fd;}

		//This is a temporary function for experiment
		LinkTask(int fd, std::string dst):type(NEXTHOP), sockfd(fd), destination(dst){}

		LinkTask(int fd, std::string dst, std::string c):type(DELAY), sockfd(fd),
				 program("ping"), destination(dst), count(c){}

		LinkTask(int fd, std::string dst, std::string band, std::string t):type(BANDWIDTH), sockfd(fd),
				 program("iperf"), protocol("-u"), destination(dst), bandwith(band), time(t)
			{bandwith.append("M");}

		enum TYPE type;
		int sockfd;
		std::string program;
		std::string protocol;
		std::string destination;
		std::string count;
		std::string bandwith;
		std::string time;
};
#endif
