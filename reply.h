/**
 * reply packet, including encode reply message to binary bits 
 * and decode binary bits to reply message.
 *
 * by wareric@163.com
 * 2018-11-07
 * 
 **/
#ifndef REPLY_H_H
#define REPLY_H_H
#include <stdint.h>
#include <string>

#define MAXLINE		4096

class Reply{
	public:
		Reply(uint32_t length, uint8_t t, std::string message);

		bool encode(void* buff, int maxlen);
		bool decode(Reply &dst, void* buff, int maxlen);

		uint32_t 	len;
		uint8_t 	type;
		std::string		msg;
};
#endif
