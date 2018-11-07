/**
 * Define a basic structure of code tool.
 *
 * by wareric@163.com
 * 2018-11-02
 */
#ifndef CODE_H_H
#define CODE_H_H
#include <string>
#include <stdint.h>
#include "linktask.h"
#include "reply.h"

#define LEN			24

#define TYPE_NEXTHOP		0
#define TYPE_BANDWITH		1
#define TYPE_DELAY		2
#define TYPE_PEER		3
#define TYPE_ERROR		255

class Code{
	public:
		virtual LinkTask decode(unsigned char buff[]) = 0;
		virtual Reply encode_next_hop(std::string network) = 0;
		virtual Reply encode_bandwith(float drop) = 0;
		virtual Reply encode_delay(float delay) = 0;
		virtual Reply encode_peer(std::string ip) = 0;
		virtual Reply encode_error() = 0;
		
		virtual ~Code(){}
	protected:
		virtual LinkTask decode_next_hop(unsigned char buff[], uint32_t len) = 0;
		virtual LinkTask decode_bandwith(unsigned char buff[], uint32_t len) = 0;
		virtual LinkTask decode_delay(unsigned char buff[], uint32_t len) = 0;
		virtual LinkTask decode_peer(unsigned char buff[], uint32_t len) = 0;
};
#endif
