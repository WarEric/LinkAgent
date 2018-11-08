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
#include <signal.h>
#include "logger.h"

Logger::Logger(std::string file):level(INFO), cli(1)
{
	if((fd = open(file.c_str(), O_WRONLY|O_CREAT|O_TRUNC|O_CLOEXEC, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)) < 0){
		std::cerr << "create logfile " << file << " fail." << std::endl;
		fd = -1;
	}
	
	if(pthread_mutex_init(&mutex, NULL) != 0)
	{
		std::cerr << "logger pthread_mutex_init fail" << std::endl;
	}
}

Logger::Logger(std::string file, LEVEL lev):level(lev), cli(1)
{
	if((fd = open(file.c_str(), O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)) < 0){
		std::cerr << "create logfile " << file << " fail." << std::endl;
		fd = -1;
	}

	if(pthread_mutex_init(&mutex, NULL) != 0)
	{
		std::cerr << "logger pthread_mutex_init fail" << std::endl;
	}
}

Logger::Logger(std::string file, LEVEL lev, int openflag):level(lev), cli(openflag)
{
	if((fd = open(file.c_str(), O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)) < 0){
		std::cerr << "create logfile " << file << " fail." << std::endl;
		fd = -1;
	}

	if(pthread_mutex_init(&mutex, NULL) != 0)
	{
		std::cerr << "logger pthread_mutex_init fail" << std::endl;
	}
}

Logger::~Logger()
{
	if(fd > 0) close(fd);
	pthread_mutex_destroy(&mutex);
}

void Logger::trace(std::string msg)
{
	pthread_mutex_lock(&mutex);

	//block all sinal
	sigset_t newmask, oldmask;
	sigfillset(&newmask);
	if(sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0)
	{
		std::string str("logger sigprocmask SIG_BLOCK error");
		if(fd > 0)
			write_log(FATAL, fd, str);
		if(cli)
			write_screen(FATAL, STDOUT_FILENO, str);
		exit(EXIT_FAILURE);
	}

	if(level <= TRACE)
	{
		if(fd > 0)
			write_log(TRACE, fd, msg);
		if(cli)
			write_screen(TRACE, STDOUT_FILENO, msg);
	}

	//resume old singal
	if(sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
	{
		std::string str("logger sigprocmask SIG_SETMASK error");
		if(fd > 0)
			write_log(FATAL, fd, str);
		if(cli)
			write_screen(FATAL, STDOUT_FILENO, str);
		exit(EXIT_FAILURE);
	}

	pthread_mutex_unlock(&mutex);
}

void Logger::debug(std::string msg)
{
	pthread_mutex_lock(&mutex);

	//block all sinal
	sigset_t newmask, oldmask;
	sigfillset(&newmask);
	if(sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0)
	{
		std::string str("logger sigprocmask SIG_BLOCK error");
		if(fd > 0)
			write_log(FATAL, fd, str);
		if(cli)
			write_screen(FATAL, STDOUT_FILENO, str);
		exit(EXIT_FAILURE);
	}

	if(level <= DEBUG)
	{
		if(fd > 0)
			write_log(DEBUG, fd, msg);
		if(cli)
			write_screen(DEBUG, STDOUT_FILENO, msg);
	}

	//resume old singal
	if(sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
	{
		std::string str("logger sigprocmask SIG_SETMASK error");
		if(fd > 0)
			write_log(FATAL, fd, str);
		if(cli)
			write_screen(FATAL, STDOUT_FILENO, str);
		exit(EXIT_FAILURE);
	}

	pthread_mutex_unlock(&mutex);
}

void Logger::info(std::string msg)
{
	pthread_mutex_lock(&mutex);

	//block all sinal
	sigset_t newmask, oldmask;
	sigfillset(&newmask);
	if(sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0)
	{
		std::string str("logger sigprocmask SIG_BLOCK error");
		if(fd > 0)
			write_log(FATAL, fd, str);
		if(cli)
			write_screen(FATAL, STDOUT_FILENO, str);
		exit(EXIT_FAILURE);
	}

	if(level <= INFO)
	{
		if(fd > 0)
			write_log(INFO, fd, msg);
		if(cli)
			write_screen(INFO, STDOUT_FILENO, msg);
	}

	//resume old singal
	if(sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
	{
		std::string str("logger sigprocmask SIG_SETMASK error");
		if(fd > 0)
			write_log(FATAL, fd, str);
		if(cli)
			write_screen(FATAL, STDOUT_FILENO, str);
		exit(EXIT_FAILURE);
	}

	pthread_mutex_unlock(&mutex);
}

void Logger::warn(std::string msg)
{
	pthread_mutex_lock(&mutex);

	//block all sinal
	sigset_t newmask, oldmask;
	sigfillset(&newmask);
	if(sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0)
	{
		std::string str("logger sigprocmask SIG_BLOCK error");
		if(fd > 0)
			write_log(FATAL, fd, str);
		if(cli)
			write_screen(FATAL, STDOUT_FILENO, str);
		exit(EXIT_FAILURE);
	}

	if(level <= WARN)
	{
		if(fd > 0)
			write_log(WARN, fd, msg);
		if(cli)
			write_screen(WARN, STDOUT_FILENO, msg);
	}

	//resume old singal
	if(sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
	{
		std::string str("logger sigprocmask SIG_SETMASK error");
		if(fd > 0)
			write_log(FATAL, fd, str);
		if(cli)
			write_screen(FATAL, STDOUT_FILENO, str);
		exit(EXIT_FAILURE);
	}

	pthread_mutex_unlock(&mutex);
}

void Logger::error(std::string msg)
{
	pthread_mutex_lock(&mutex);

	//block all sinal
	sigset_t newmask, oldmask;
	sigfillset(&newmask);
	if(sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0)
	{
		std::string str("logger sigprocmask SIG_BLOCK error");
		if(fd > 0)
			write_log(FATAL, fd, str);
		if(cli)
			write_screen(FATAL, STDOUT_FILENO, str);
		exit(EXIT_FAILURE);
	}

	if(level <= ERROR)
	{
		if(fd > 0)
			write_log(ERROR, fd, msg);
		if(cli)
			write_screen(ERROR, STDOUT_FILENO, msg);
	}

	//resume old singal
	if(sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
	{
		std::string str("logger sigprocmask SIG_SETMASK error");
		if(fd > 0)
			write_log(FATAL, fd, str);
		if(cli)
			write_screen(FATAL, STDOUT_FILENO, str);
		exit(EXIT_FAILURE);
	}

	pthread_mutex_unlock(&mutex);
}

void Logger::fatal(std::string msg)
{
	pthread_mutex_lock(&mutex);

	//block all sinal
	sigset_t newmask, oldmask;
	sigfillset(&newmask);
	if(sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0)
	{
		std::string str("logger sigprocmask SIG_BLOCK error");
		if(fd > 0)
			write_log(FATAL, fd, str);
		if(cli)
			write_screen(FATAL, STDOUT_FILENO, str);
		exit(EXIT_FAILURE);
	}

	if(level <= FATAL)
	{
		if(fd > 0)
			write_log(FATAL, fd, msg);
		if(cli)
			write_screen(FATAL, STDOUT_FILENO, msg);
	}

	//resume old singal
	if(sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
	{
		std::string str("logger sigprocmask SIG_SETMASK error");
		if(fd > 0)
			write_log(FATAL, fd, str);
		if(cli)
			write_screen(FATAL, STDOUT_FILENO, str);
		exit(EXIT_FAILURE);
	}

	pthread_mutex_unlock(&mutex);
}

int Logger::write_log(LEVEL level, int fd, std::string msg)
{
	switch(level){
		case TRACE:
			msg.insert(0, " TRACE: ");
			break;
		case DEBUG:
			msg.insert(0, " DEBUG: ");
			break;
		case INFO:
			msg.insert(0, " INFO:  ");
			break;
		case WARN:
			msg.insert(0, " WARN:  ");
			break;
		case ERROR:
			msg.insert(0, " ERROR: ");
			break;
		case FATAL:
			msg.insert(0, " FATAL: ");
			break;
		default:
			break;
	}
	msg.insert(0, logtime());
	msg.append("\n");
	int n = write(fd, msg.c_str(), msg.size());
	return n;
}

int Logger::write_screen(LEVEL level, int fd, std::string msg)
{
	switch(level){
		case TRACE:
			msg.insert(0, " TRACE: ");
			msg.insert(0, logtime());
			msg.insert(0, "\033[36m");			//deepgreen
			break;
		case DEBUG:
			msg.insert(0, " DEBUG: ");
			msg.insert(0, logtime());
			msg.insert(0, "\033[32m");			//green
			break;
		case INFO:
			msg.insert(0, " INFO:  ");
			msg.insert(0, logtime());
			msg.insert(0, "\033[34m");			//blue
			break;
		case WARN:
			msg.insert(0, " WARN:  ");
			msg.insert(0, logtime());
			msg.insert(0, "\033[35m");			//purple
			break;
		case ERROR:
			msg.insert(0, " ERROR: ");
			msg.insert(0, logtime());
			msg.insert(0, "\033[33m");			//yellow
			break;
		case FATAL:
			msg.insert(0, " FATAL: ");
			msg.insert(0, logtime());
			msg.insert(0, "\033[31m");			//red
			break;
		default:
			break;
	}
	msg.append("\033[0m\n");
	int n = write(fd, msg.c_str(), msg.size());
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
