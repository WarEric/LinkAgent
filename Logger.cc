/**
 * by wareric@163.com 2018-10-21
 **/
#include <iostream>
#include <cstring>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include "Logger.h"

Logger::Logger(std::string file):level(INFO), cli(1)
{
	if((fd = open(file.c_str(), O_WRONLY|O_CREAT|O_TRUNC|O_CLOEXEC, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)) < 0){
		std::cerr << "create logfile " << file << " fail." << std::endl;
		fd = -1;
	}
}

Logger::Logger(std::string file, LEVEL lev):level(lev), cli(1)
{
	if((fd = open(file.c_str(), O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)) < 0){
		std::cerr << "create logfile " << file << " fail." << std::endl;
		fd = -1;
	}
}

Logger::Logger(std::string file, LEVEL lev, int openflag):level(lev), cli(openflag)
{
	if((fd = open(file.c_str(), O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)) < 0){
		std::cerr << "create logfile " << file << " fail." << std::endl;
		fd = -1;
	}
}

Logger::~Logger()
{
	if(fd > 0) close(fd);
}

int Logger::write_log(LEVEL level, int fd, const void *buf, size_t count)
{
	//I got the time code from internet, though it isn't efficient.
	struct timeval  tv;
	char	timeArray[40];
	std::stringstream ss;

        gettimeofday(&tv, NULL);
        memset(timeArray, 0, sizeof(timeArray));
        strftime(timeArray, sizeof(timeArray) - 1, "%F %T", localtime(&tv.tv_sec));
	ss << std::string(timeArray) << "." << tv.tv_usec;

	write(fd, ss.str().c_str(), ss.str().size());
	switch(level){
		case TRACE:
			write(fd, " TRACE: ", strlen(" TRACE: "));
			break;
		case DEBUG:
			write(fd, " DEBUG: ", strlen(" DEBUG: "));
			break;
		case INFO:
			write(fd, " INFO: ", strlen(" INFO: "));
			break;
		case WARN:
			write(fd, " WARN: ", strlen(" WARN: "));
			break;
		case ERROR:
			write(fd, " ERROR: ", strlen(" ERROR: "));
			break;
		case FATAL:
			write(fd, " FATAL: ", strlen(" FATAL: "));
			break;
		default:
			break;
	}
	int n = write(fd, buf, count);
	write(fd, "\n", strlen("\n"));
	return n;
}

void Logger::trace(std::string msg)
{
	if(fd < 0) return;

	if(level <= TRACE)
	{
		write_log(TRACE, fd, msg.c_str(), msg.size());
		if(cli)
			write_log(TRACE, STDOUT_FILENO, msg.c_str(), msg.size());
	}
}

void Logger::debug(std::string msg)
{
	if(fd < 0) return;

	if(level <= DEBUG)
	{
		write_log(DEBUG, fd, msg.c_str(), msg.size());
		if(cli)
			write_log(DEBUG, STDOUT_FILENO, msg.c_str(), msg.size());
	}
}

void Logger::info(std::string msg)
{
	if(fd < 0) return;

	if(level <= INFO)
	{
		write_log(INFO, fd, msg.c_str(), msg.size());
		if(cli)
			write_log(INFO, STDOUT_FILENO, msg.c_str(), msg.size());
	}
}

void Logger::warn(std::string msg)
{
	if(fd < 0) return;

	if(level <= WARN)
	{
		write_log(WARN, fd, msg.c_str(), msg.size());
		if(cli)
			write_log(WARN, STDOUT_FILENO, msg.c_str(), msg.size());
	}
}

void Logger::error(std::string msg)
{
	if(fd < 0) return;

	if(level <= ERROR)
	{
		write_log(ERROR, fd, msg.c_str(), msg.size());
		if(cli)
			write_log(ERROR, STDOUT_FILENO, msg.c_str(), msg.size());
	}
}

void Logger::fatal(std::string msg)
{
	if(fd < 0) return;

	if(level <= FATAL)
	{
		write_log(FATAL, fd, msg.c_str(), msg.size());
		if(cli)
			write_log(FATAL, STDOUT_FILENO, msg.c_str(), msg.size());
	}
}
