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
#include "logger.h"

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

void Logger::trace(std::string msg)
{
	if(fd < 0) return;

	if(level <= TRACE)
	{
		write_log(TRACE, fd, msg.c_str(), msg.size());
		if(cli)
			write_screen(TRACE, STDOUT_FILENO, msg.c_str(), msg.size());
	}
}

void Logger::debug(std::string msg)
{
	if(fd < 0) return;

	if(level <= DEBUG)
	{
		write_log(DEBUG, fd, msg.c_str(), msg.size());
		if(cli)
			write_screen(DEBUG, STDOUT_FILENO, msg.c_str(), msg.size());
	}
}

void Logger::info(std::string msg)
{
	if(fd < 0) return;

	if(level <= INFO)
	{
		write_log(INFO, fd, msg.c_str(), msg.size());
		if(cli)
			write_screen(INFO, STDOUT_FILENO, msg.c_str(), msg.size());
	}
}

void Logger::warn(std::string msg)
{
	if(fd < 0) return;

	if(level <= WARN)
	{
		write_log(WARN, fd, msg.c_str(), msg.size());
		if(cli)
			write_screen(WARN, STDOUT_FILENO, msg.c_str(), msg.size());
	}
}

void Logger::error(std::string msg)
{
	if(fd < 0) return;

	if(level <= ERROR)
	{
		write_log(ERROR, fd, msg.c_str(), msg.size());
		if(cli)
			write_screen(ERROR, STDOUT_FILENO, msg.c_str(), msg.size());
	}
}

void Logger::fatal(std::string msg)
{
	if(fd < 0) return;

	if(level <= FATAL)
	{
		write_log(FATAL, fd, msg.c_str(), msg.size());
		if(cli)
			write_screen(FATAL, STDOUT_FILENO, msg.c_str(), msg.size());
	}
}

int Logger::write_log(LEVEL level, int fd, const void *buf, size_t count)
{
	std::string time = logtime();
	write(fd, time.c_str(), time.size());
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

int Logger::write_screen(LEVEL level, int fd, const void *buf, size_t count)
{
	std::string time = logtime();
	switch(level){
		case TRACE:
			time.insert(0, "\033[36m");			//deepgreen
			write(fd, time.c_str(), time.size());
			write(fd, " TRACE: ", strlen(" TRACE: "));
			break;
		case DEBUG:
			time.insert(0, "\033[32m");			//green
			write(fd, time.c_str(), time.size());
			write(fd, " DEBUG: ", strlen(" DEBUG: "));
			break;
		case INFO:
			time.insert(0, "\033[34m");			//blue
			write(fd, time.c_str(), time.size());
			write(fd, " INFO: ", strlen(" INFO: "));
			break;
		case WARN:
			time.insert(0, "\033[35m");			//purple
			write(fd, time.c_str(), time.size());
			write(fd, " WARN: ", strlen(" WARN: "));
			break;
		case ERROR:
			time.insert(0, "\033[33m");			//yellow
			write(fd, time.c_str(), time.size());
			write(fd, " ERROR: ", strlen(" ERROR: "));
			break;
		case FATAL:
			time.insert(0, "\033[31m");			//red
			write(fd, time.c_str(), time.size());
			write(fd, " FATAL: ", strlen(" FATAL: "));
			break;
		default:
			break;
	}
	int n = write(fd, buf, count);
	write(fd, "\033[0m\n", strlen("\033[0m\n"));
	return n;
}

/**
 * I got the time code from internet, though it isn't efficient.
 **/
std::string Logger::logtime()
{
	struct timeval  tv;
	char	timeArray[40];
	std::stringstream ss;

        gettimeofday(&tv, NULL);
        memset(timeArray, 0, sizeof(timeArray));
        strftime(timeArray, sizeof(timeArray) - 1, "%F %T", localtime(&tv.tv_sec));
	ss << std::string(timeArray) << "." << tv.tv_usec;
	
	return ss.str();
}
