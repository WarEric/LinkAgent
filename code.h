/**
 * Define a basic structure of code tool.
 *
 * by wareric@163.com
 * 2018-11-02
 */
#ifndef CODE_H_H
#define CODE_H_H
#include <string>
#include "linkagent.h"

#define TYPE_NEXTHOP		0
#define TYPE_BANDWIDTH		1
#define TYPE_DELAY		2
#define TYPE_ERROR		255

class Code{
	public:
		virtual LinkTask decode(char buff[]) = 0;
		virtual std::string encode_next_hop(std::string) = 0;
		virtual std::string encode_bandwith(float drop) = 0;
		virtual std::string encode_delay(float delay) = 0;
		virtual std::string encode_error() = 0;
		
		virtual ~Code(){}
	protected:
		virtual LinkTask decode_next_hop(char buff[], int len) = 0;
		virtual LinkTask decode_bandwith(char buff[], int len) = 0;
		virtual LinkTask decode_delay(char buff[], int len) = 0;
};
#endif
