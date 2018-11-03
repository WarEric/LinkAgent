/**
	The full name of linkagent is Link State Agent. It can handle
link bandwith or delay requests from clients. By using the well-known link
bandwith detecting program "iperf" and system network tool "ping", we get 
bandwith and delay data. The linkagent exposes a tcp socket for users. While
receving a link request from tcp socket, linkagent will transfor your request
to iperf or ping command and execute it in a child process. The linkagent will
analysis the result from child process and reply it to client. From previous 
introduction, you may guess that "iperf" and "ping" program must be installed
in your system. That's right, they are assumed to be installed in your system.
	This program supports high concurrent connections(I don't test how many
connections it support).

	by wareric@163.com 2018-10-19
**/
#include <iostream>
#include <string>
#include <string.h>
#include <unordered_map>
#include <queue>
#include <list>
#include <memory>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include "strings.h"
#include "linkagent.h"
#include "simplecode.h"
#include "Logger.h"

#define STRERR(errno) (std::to_string(errno)+" "+std::string(strerror(errno)))
#define MAXLINE		4096
#define PORT		65530
#define BACKLOG		10
#define MAXEPOLL	100
#define LEN		24

using std::cout;
using std::cerr;
using std::cin;
using std::endl;
using std::string;
using std::to_string;
using std::unordered_map;
using std::make_pair;
using std::queue;
using std::list;
using std::shared_ptr;
using std::make_shared;

void init();
int run(int listen_fd);
void schedule();
pid_t exec_bandwith_task(LinkTask task);
pid_t exec_delay_task(LinkTask task);
void commit_task(LinkTask task);
void clear_pipe(int fd);
void clear_socket(int sockfd);
void myexit();

int create_iperf_server(string logfile);
int establish_tcp_listener(uint16_t port, int backlog);

void handle_signal();
void sigint_handler(int signo);
void sigchld_handler(int signo);

void add_reply(int sockfd, string result);
float analysis_bandwith(int fd);
float analysis_delay(int fd);

ssize_t read_line(int fd, void *vptr, ssize_t maxlen);
ssize_t readn(int fd, void *vptr, size_t n);
ssize_t writen(int fd, const void *vptr, size_t n);
void set_cloexec(int fd);

int listenfd = -1;
int epoll_fd = -1;
pid_t iperf_server_pid = -1;
Logger* logger = nullptr;

unordered_map<string, queue<LinkTask>>	task_map;			//map<string, queue<task>>
unordered_map<int, Client>		sockfd_client_map;		//map<socketfd, client>
unordered_map<int, int>			bandwith_pipefd_sockfd_map;	//map<bandwithpipefd, socketfd>
unordered_map<int, int>			delay_pipefd_sockfd_map;	//map<delaypipefd, socketfd>
unordered_map<string, int>		task_pipefd_map;		//map<string, pipe> pipe = bandwithxxx + delayxxx
unordered_map<string, pid_t>		task_pid_map;			//map<string, pid_t>
queue<int>				wait_pipe;			//queue<pipe> wait for add in epoll

int main(int argc, char *argv[])
{	
	logger = new Logger("linkagent.log", Logger::TRACE, 1);

	logger->info("starting up.......");
	handle_signal();

	init();
	if(run(listenfd) < 0)
		logger->fatal("linkagent run() meets critical internal error");
	myexit();
	return 0;
}

void init()
{	
	//create iperf server
	if((iperf_server_pid = create_iperf_server("iperf.log")) < 0)
	{
		logger->fatal("create iperf server failed");
		myexit();
	}
	logger->trace("iperf_server_pid = " + to_string(iperf_server_pid));

	//establish socket listen
	if((listenfd = establish_tcp_listener(PORT, BACKLOG))  < 0)
	{
		logger->fatal("establist tcp listener failed");
		myexit();
	}
}

int run(int listen_fd)
{
	int conn_fd;
	int cur_fds;
	int wait_fds;
	struct sockaddr_in cliaddr;
	struct epoll_event ev;
	struct epoll_event evs[MAXEPOLL];

	socklen_t len = sizeof(cliaddr);
	if((epoll_fd = epoll_create(MAXEPOLL)) < 0)
	{
		logger->fatal("epoll_create error " + STRERR(errno));
	}
	ev.events = EPOLLIN;
	ev.data.fd = listen_fd;
	if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &ev) < 0)
	{
		logger->fatal("epoll_ctl error : " + STRERR(errno));
		return -1;
	}

	shared_ptr<Code> code = make_shared<SimpleCode>();
	cur_fds = 1;
	while(true)
	{
		if((wait_fds = epoll_wait(epoll_fd, evs, cur_fds, -1)) == -1)
		{
			if(errno == EINTR)
				continue;
			else{
				logger->fatal("epoll_wait error " + STRERR(errno));
				return -1;
			}
		}

		char buff[LEN];
		for(int i = 0; i < wait_fds; i++)
		{	
			//handle new connection request
			if(evs[i].data.fd == listen_fd && cur_fds < MAXEPOLL)
			{
				if((conn_fd = accept(listen_fd, (struct sockaddr *)&cliaddr, &len)) == -1)
				{
					logger->error("accpet error");
				}
				set_cloexec(conn_fd);

				int port = ntohs(cliaddr.sin_port);
				string addr = string(inet_ntoa(cliaddr.sin_addr));
				logger->info("client " + addr +":"+ to_string(port) + " connected");

				ev.events = EPOLLIN | EPOLLOUT;
				ev.data.fd = conn_fd;
				if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, conn_fd, &ev) < 0)
				{
					logger->error("epoll error " + errno);
					logger->warn("client "+ addr + ":"+ to_string(port) + " disconnected");
					close(conn_fd);
					continue;
				}
				++cur_fds;
				sockfd_client_map.insert(make_pair(conn_fd, Client(conn_fd)));
				continue;
			}

			//handle socket io ready
			auto iter = sockfd_client_map.find(evs[i].data.fd);
			if(iter != sockfd_client_map.end())
			{
				if(evs[i].events & EPOLLIN)	//socket read
				{
					if(readn(iter->first, buff, LEN) != LEN)
					{
						logger->error("readn from socket error");
						clear_socket(iter->first);

						ev.events = EPOLLIN | EPOLLOUT;
						ev.data.fd = iter->first;
						if(epoll_ctl(epoll_fd, EPOLL_CTL_DEL, iter->first, &ev) < 0)
						{
							logger->fatal("epoll_ctl error : " + STRERR(errno));
							return -1;
						}
						continue;
					}else{
						LinkTask linktask = code->decode(buff);
						if(linktask.type == LinkTask::ERROR)
						{
							logger->error("decode error");
							add_reply(iter->first, code->encode_error());
						}else{
							linktask.sockfd = iter->first;
							commit_task(linktask);
							schedule();
						}
					}
				}else if(evs[i].events & EPOLLOUT){//socket write
					if(!iter->second.reply.empty())
					{
						memset(buff, 0, LEN);
						string result = iter->second.reply.front();
						iter->second.reply.pop();
						memcpy(buff, result.c_str(), result.size());
						if(writen(iter->first, buff, LEN) != LEN)
						{
							logger->error("writen error");
							clear_socket(iter->first);

							ev.events = EPOLLIN | EPOLLOUT;
							ev.data.fd = iter->first;
							if(epoll_ctl(epoll_fd, EPOLL_CTL_DEL, iter->first, &ev) < 0)
							{
								logger->fatal("epoll_ctl error : " + STRERR(errno));
								return -1;
							}
							continue;
						}
					}
				}
				continue;
			}

			//handle bandwith task io ready
			auto it = bandwith_pipefd_sockfd_map.find(evs[i].data.fd);
			if(it != bandwith_pipefd_sockfd_map.end())
			{
				if(evs[i].events & EPOLLIN)
				{
					float rate = analysis_bandwith(it->first);
					if(rate < 0)
					{
						logger->error("analysis bandwith result error");
						add_reply(it->second, code->encode_error());
					}else{
						add_reply(it->second, code->encode_bandwith(rate));

					}
					
					ev.events = EPOLLIN;
					ev.data.fd = it->first;
					if(epoll_ctl(epoll_fd, EPOLL_CTL_DEL, it->first, &ev) < 0)
					{
						logger->fatal("epoll_ctl error : " + STRERR(errno));
						return -1;
					}
					clear_pipe(it->first);
				}
				schedule();
				continue;
			}

			//handle delay task io ready
			it = delay_pipefd_sockfd_map.find(evs[i].data.fd);
			if(it != delay_pipefd_sockfd_map.end())
			{
				if(evs[i].events & EPOLLIN)
				{
					float delay = analysis_delay(it->first);
					if(delay < 0)
					{
						logger->error("analysis delay result error");
						add_reply(it->second, code->encode_error());
					}

					ev.events = EPOLLIN;
					ev.data.fd = it->first;
					if(epoll_ctl(epoll_fd, EPOLL_CTL_DEL, it->first, &ev) < 0)
					{
						logger->fatal("epoll_ctl error : " + STRERR(errno));
						return -1;
					}
					clear_pipe(it->first);
				}
				schedule();
				continue;
			}

			logger->error("epoll event fd miss match handler");
		}
		
		//add wait pipe in epoll
		while(cur_fds < MAXEPOLL && !wait_pipe.empty())
		{
			ev.events = EPOLLIN;
			ev.data.fd = wait_pipe.front();
			if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, wait_pipe.front(), &ev) < 0)
			{
				logger->error("epoll_ctl error " + STRERR(errno));
				//rm contains map and reply error in client
				close(wait_pipe.front());
			}
			wait_pipe.pop();
		}
	}

}

void schedule()
{
	list<string> clean;
	for(auto iter = task_map.begin(); iter != task_map.end(); ++iter)
	{
		auto it = task_pid_map.find(iter->first);
		if(it != task_pid_map.end())
			continue;

		LinkTask task = iter->second.front();
		iter->second.pop();
		if(iter->second.empty())
			clean.push_back(iter->first);

		if(task.type == LinkTask::BANDWIDTH)
		{
			if(exec_bandwith_task(task) < 0)
			{
				logger->error("schedule bandwidth task for error");
				SimpleCode code;
				add_reply(task.sockfd, code.encode_error());
			}else{
				logger->debug("task " + iter->first + " start run");
			}
			continue;
		}

		if(task.type == LinkTask::DELAY)
		{
			if(exec_delay_task(task) < 0)
			{
				logger->error("schedule delay task for error");
				SimpleCode code;
				add_reply(task.sockfd, code.encode_error());
			}else{
				logger->debug("task " + iter->first + " start run");
			}
			continue;
		}
	}

	for(string str : clean)
		task_map.erase(str);
}

pid_t exec_bandwith_task(LinkTask task)
{
	pid_t pid;
	int fd[2];

	if(pipe(fd) < 0)
	{
		logger->error("exec bandwith task pipe fail");
		return -1;
	}

	if((pid = fork()) < 0){
		logger->error("exec bandwith task fork fail");
		return -1;
	}else if(pid == 0){ // child
		close(STDIN_FILENO);
		close(fd[0]);
		dup2(fd[1], STDOUT_FILENO);
		dup2(fd[1], STDERR_FILENO);
		close(fd[1]);
		if(execlp(task.program.c_str(), task.program.c_str(), task.protocol.c_str(), "-c", task.destination.c_str(),
			"-b", task.bandwith.c_str(), "-t", task.time.c_str(), NULL) < 0){
			//signal parent
			logger->error("exec bandwith task execlp error");
			exit(EXIT_FAILURE);
		}
	}

	close(fd[1]);
	set_cloexec(fd[0]);

	//modify global data
	wait_pipe.push(fd[0]);
	task_pid_map.emplace(task.destination, pid);
	task_pipefd_map.emplace(task.destination, fd[0]);
	bandwith_pipefd_sockfd_map.emplace(fd[0], task.sockfd);

	logger->debug("exec bandwith task success.");
	return pid;
}

pid_t exec_delay_task(LinkTask task)
{
	pid_t pid;
	int fd[2];

	if(pipe(fd) < 0)
	{
		logger->error("exec delay task pipe fail");
		return -1;
	}

	if((pid = fork()) < 0){
		logger->error("exec delay task fork fail");
		return -1;
	}else if(pid == 0){ // child
		close(STDIN_FILENO);
		close(fd[0]);
		dup2(fd[1], STDOUT_FILENO);
		dup2(fd[1], STDERR_FILENO);
		close(fd[1]);
		if(execlp(task.program.c_str(), task.program.c_str(), "-c", task.count.c_str(),
			task.destination.c_str(), NULL) < 0){
			//signal parent
			logger->error("exec delay task execlp error");
			exit(EXIT_FAILURE);
		}
	}

	close(fd[1]);
	set_cloexec(fd[0]);

	//modify global data
	wait_pipe.push(fd[0]);
	task_pid_map.emplace(task.destination, pid);
	task_pipefd_map.emplace(task.destination, fd[0]);
	delay_pipefd_sockfd_map.emplace(fd[0], task.sockfd);

	logger->debug("exec delay task success.");
	return pid;
}

void commit_task(LinkTask task)
{
	//finish nexthop task here, this is a temporary function.
	//
	//
	//
	//
	//
	//

	auto it = task_map.find(task.destination);
	if(it == task_map.end())
	{
		queue<LinkTask> myque;
		myque.push(task);
		task_map.insert(make_pair(task.destination, myque));
	}else{
		it->second.push(task);
	}

	logger->trace("commit a task " + task.destination );
}

void clear_pipe(int fd)
{
	unordered_map<string, int>::iterator it;
	for(it = task_pipefd_map.begin(); it != task_pipefd_map.end(); ++it)
	{
		if(it->second == fd)
			break;
	}

	string task;
	if(it != task_pid_map.end())
		task = it->first;

	task_pipefd_map.erase(task);
	task_pid_map.erase(task);
	bandwith_pipefd_sockfd_map.erase(fd);
	delay_pipefd_sockfd_map.erase(fd);
}

void clear_socket(int sockfd)
{
	sockfd_client_map.erase(sockfd);

	logger->info("sockfd " + to_string(sockfd) + " disconnected.");
	close(sockfd);
}

void myexit()
{
	//close listen socket
	if(listenfd > 0)
	{
		if(close(listenfd) < 0)
			logger->warn("close listen socket fail");
		else
			logger->info("listen socket closed");
		
		listenfd = -1;
	}

	//close all socket connections
	for(auto it = sockfd_client_map.begin(); it != sockfd_client_map.end(); ++it)
	{
		//we can get ip and port in here
		if(close(it->first) < 0)
			logger->warn("close socket connection fail");
		
	}
	logger->info("close all sockets connection");

	//kill all running tasks
	for(auto it = task_pid_map.begin(); it != task_pid_map.end(); ++it)
	{
		if(kill(it->second, SIGTERM) < 0)
		{
			logger->warn("stop task " + it->first + " fail");
			continue;
		}

		//assume all waitpid will success, I just don't know how to write my waitpid() with timeout.
		if(waitpid(it->second, NULL, 0) < 0)
		{
			logger->warn("retrevie task " + it->first + " dead state fail");
		}else{
			logger->info("task " + it->first + " closed");
		}
	}
	
	//close all pipefds
	for(auto it = task_pipefd_map.begin(); it != task_pipefd_map.end(); ++it)
	{
		if(close(it->second) < 0)
			logger->warn("close task " + it->first + " pipefd failed");
		else{
			logger->debug("task " + it->first + " pipefd closed");
		}
	}

	//close epoll
	if(epoll_fd > 0)
	{
		if(close(epoll_fd) < 0)
			logger->warn("close epoll_fd fail");
		epoll_fd = -1;
	}

	//kill iperf server
	if(iperf_server_pid > 0)
	{
		if(kill(iperf_server_pid, SIGKILL) < 0)
			logger->error("close process " + to_string(iperf_server_pid) + " iperf server fail");
		else{
			if(waitpid(iperf_server_pid, NULL, 0) < 0)
			{
				logger->warn("retrevie iperf server dead state fail");
			}else{
				logger->info("iperf server closed");
			}
		}
		iperf_server_pid = -1;
	}
	
	delete logger;
	logger = nullptr;
	exit(0);
}

/**
* create a background iperf server.
* return pid_t
* It return iperf pid when successfully
* create iperf background progress,
* otherwise it return -1.
**/
int create_iperf_server(string logfile)
{
	pid_t iperf_pid;
	int logfd;

	unlink(logfile.c_str());
	if((logfd = creat(logfile.c_str(),S_IRUSR|S_IRGRP|S_IROTH)) < 0)
	{
		logger->warn("create file "+logfile+" error");
		return -1;
	}
	logger->info("create file "+logfile);

	if((iperf_pid = fork()) < 0){
		logger->fatal("create iperf server fork error");
		close(logfd);
		return -1;
	}else if(iperf_pid == 0){ 			//child
		close(STDIN_FILENO);
		dup2(logfd, STDOUT_FILENO);
		dup2(logfd, STDERR_FILENO);
		close(logfd);
		if(execlp("iperf", "iperf", "-u", "-s", NULL) < 0){
			logger->fatal("create iperf server execlp error");
			//it should signal parent agent
			exit(1);
		}
	}

	close(logfd);
	logger->info("iperf server is created");
	return iperf_pid;
}

int establish_tcp_listener(uint16_t port, int backlog)
{
	struct sockaddr_in servaddr;
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(port);

	int fd;
	if((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		logger->fatal("socket error " + errno);
		return -1;
	}

	int on = 1;
	int result = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	if(-1 == result) logger->warn("set socket reuseaddr fail");

	if(bind(fd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1)
	{
		logger->fatal("bind error " + to_string(errno));
		return -1;
	}

	if(listen(fd, backlog) == -1)
	{
		logger->fatal("listen error" + to_string(errno));
		return -1;
	}
	logger->info("linkagent is listening on port " + to_string(port));
	return fd;
}

void handle_signal()
{
	struct sigaction sa;
	
	sa.sa_handler = sigint_handler;
	sigfillset(&sa.sa_mask);
	if(sigaction(SIGINT, &sa, NULL) == -1){
		logger->fatal("add sigaction SIGINT fail");
		myexit();
	}

	struct sigaction sga;
	sga.sa_handler = sigchld_handler;
	sigemptyset(&sga.sa_mask);
	if(sigaction(SIGCHLD, &sga, NULL) == -1){
		logger->fatal("add sigaction SIGCHLD fail");
		myexit();
	}
}

void sigint_handler(int signo)
{	
	logger->debug("receive SIGINT");
	myexit();
}

void sigchld_handler(int signo)
{
	logger->debug("receive SIGCHLD");
	pid_t pid;
	while((pid = waitpid(-1, NULL, WNOHANG)) > 0)
	{
		logger->trace("child process " + to_string(pid) + " dead");
	}
}

void add_reply(int sockfd, string result)
{
	auto it = sockfd_client_map.find(sockfd);
	if(it == sockfd_client_map.end())
	{
		logger->warn("no client map with sockfd " + to_string(sockfd));
		return;
	}

	it->second.reply.push(result);
}

float analysis_bandwith(int fd)
{
	char line[MAXLINE];
	int n = 0;
	string lastline, result;
	while((n = read_line(fd, line, MAXLINE)) > 0)
	{
		if(lastline.find("Server Report:") != string::npos)
		{
			result = string(line);
			while(read(fd, line, MAXLINE) > 0)
				;
			break;
		}
		lastline = string(line);
	}

	if(result.empty())
		return -1;

	size_t pos = result.find('(') + 1;
	size_t len = result.find(')') - pos;
	result = result.substr(pos, len);
	return stoi(result);
}

float analysis_delay(int fd)
{
	char line[MAXLINE];
	size_t pos;
	int n = 0, count = 0;
	float time = 0.0f;
	while((n = read_line(fd, line, MAXLINE)) > 0)
	{
		string str(line);
		if((pos = str.find("time=")) == string::npos)
			continue;

		time += stof(str.substr(pos+6, str.find("ms")-pos-6-1));
		count++;
	}

	if(time == 0)
		return -1;
	time /= 2.0;
	return time/(float)count;
}

/**
 * I copy this from UNIX Network Programming [W. Richard Stevens].
 * It may not the best way. :)
 *
**/
ssize_t read_line(int fd, void *vptr, ssize_t maxlen)
{
	ssize_t n, rc;
	char c, *ptr;

	ptr = (char *)vptr;
	for (n = 1; n < maxlen; n++)
	{
		again:
		if ((rc = read(fd, &c, 1)) == 1)
		{
			*ptr++ = c;
			if (c == '\n')
				break;
		}
		else if (rc == 0)
		{
			*ptr = 0;
			return(n - 1);
		} 
		else
		{
			if (errno == EINTR)
				goto again;
			return(-1);
		}
	}
	*ptr = 0;
	return(n);
}

/**
 * I copy this from UNIX Network Programming [W. Richard Stevens].
 *
**/
ssize_t readn(int fd, void *vptr, size_t n)
{
	size_t nleft;
	ssize_t nread;
	char *ptr;
	ptr = (char *)vptr;
	nleft = n;
	while(nleft > 0){
		if((nread = read(fd, ptr, nleft)) < 0){
			if(errno == EINTR)
				nread = 0;	/* call read again() */
			else
				return (-1);
		}else if(nread == 0)
			break;			/* EOF */
		nleft -= nread;
		ptr += nread;
	}
	return (n-nleft);			/* return >= 0 */
}

/**
 * I copy this from UNIX Network Programming [W. Richard Stevens].
 *
**/
ssize_t writen(int fd, const void *vptr, size_t n)
{
	size_t nleft;
	ssize_t nwritten;
	const char *ptr;

	ptr = (char *)vptr;
	nleft = n;
	while(nleft > 0){
		if((nwritten = write(fd, ptr, nleft)) <= 0){
			if(nwritten < 0 && errno == EINTR)
				nwritten = 0;			/* call write() again */
			else
				return (-1);			/* error */
		}
		nleft -= nwritten;
		ptr += nwritten;
	}
	return (n);
}

void set_cloexec(int fd)
{
	int flags = fcntl(fd, F_GETFD);
	flags |= FD_CLOEXEC;
	for(int i = 5; i > 0 && fd > 0; --i)
	{
		if(fcntl(fd, F_SETFD, flags) < 0)
			logger->warn("set fd " + to_string(fd) + " close-on-exec error");
	}
}
