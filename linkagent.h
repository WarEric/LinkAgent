/**
 * Define some data class for linkagent.
 * by wareric@163.com 2018-10-22
**/

#ifndef LINKAGENT_H_H
#define LINKAGENT_H_H

#include <string>
#include <queue>
#include <unistd.h>
#include "reply.h"

class Client{
	public:
		Client(int fd):socketfd(fd){}

		int socketfd;
		std::queue<Reply> reply;
};
#endif
