/**
 * A simple log class. It record log messages to a file in current directory.
 * I try my best to make it as easy as log4j(java) library.
 * by wareric@163.com 2018-10-21
 **/
#ifndef LOGGER_H_H
#define LOGGER_H_H
#include <string>

class Logger{
	public:
		enum LEVEL{
			TRACE,
			DEBUG,
			INFO,
			WARN,
			ERROR,
			FATAL
		};
	
		Logger(std::string file);
		Logger(std::string file, LEVEL lev);
		Logger(std::string file, LEVEL lev, int openflag);

		~Logger();
		
		void trace(std::string msg);
		void debug(std::string msg);
		void info(std::string msg);
		void warn(std::string msg);
		void error(std::string msg);
		void fatal(std::string msg);
	private:
		Logger(const Logger&);			// forbide copy construct
		Logger& operator=(const Logger&);	// forbide assignment contruct

		//this will add time and level in head, insert a new line
		int write_log(LEVEL level, int fd, const void *buf, size_t count);

		int level;				// trace < debug < info < warn < error < fatal
		int fd;					// logfd
		int cli;				// 1 represent print to CLI, 0 represent not.
};
#endif
