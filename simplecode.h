/**
 * Define a simple code way, just to finish the experiment as soon
 * as possible. I will rewrite a perfect code way lately.
 * 
 * by wareric@163.com
 * 2018-11-02
 */
#ifndef SIMPLECODE_H_H
#define SIMPLECODE_H_H
#include <string>
#include "code.h"

class SimpleCode : public Code{
	public:
		virtual LinkTask decode(unsigned char buff[]) override;
		virtual Reply encode_next_hop(std::string network) override;
		virtual Reply encode_bandwith(float drop) override;
		virtual Reply encode_delay(float delay) override;
		virtual Reply encode_peer(std::string ip) override;
		virtual Reply encode_error() override;
		
		virtual ~SimpleCode(){}
	protected:
		virtual LinkTask decode_next_hop(unsigned char buff[], uint32_t len) override;
		virtual LinkTask decode_bandwith(unsigned char buff[], uint32_t len) override;
		virtual LinkTask decode_delay(unsigned char buff[], uint32_t len) override;
		virtual LinkTask decode_peer(unsigned char buff[], uint32_t len) override;
};
#endif
