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
		virtual LinkTask decode(char buff[]) override;
		virtual std::string encode_next_hop(std::string) override;
		virtual std::string encode_bandwith(float drop) override;
		virtual std::string encode_delay(float delay) override;
		virtual std::string encode_error() override;
		
		virtual ~SimpleCode(){}
	protected:
		virtual LinkTask decode_next_hop(char buff[], int len) override;
		virtual LinkTask decode_bandwith(char buff[], int len) override;
		virtual LinkTask decode_delay(char buff[], int len) override;
};
#endif
