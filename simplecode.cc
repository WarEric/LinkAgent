/**
 * Some function's defination of SimpleCode class.
 
 * by wareric@163.com
 * 2018-11-02
 */
#include <cstring>
#include <string.h>
#include "simplecode.h"

using std::string;
using std::to_string;

LinkTask SimpleCode::decode(unsigned char buff[])
{
	uint32_t len;
	unsigned char type;
	memset(&len, 0, sizeof(len));
	memset(&type, 0, sizeof(type));

	memcpy(&len, buff, 4);
	memcpy(&type, buff+4, 1);

	switch(type){
		case TYPE_NEXTHOP:
			return decode_next_hop(buff, len);
		case TYPE_BANDWITH:
			return decode_bandwith(buff, len);
		case TYPE_DELAY:
			return decode_delay(buff, len);
		case TYPE_PEER:
			return decode_peer(buff, len);
		default:
			return LinkTask();
	}
	return LinkTask();
}

Reply SimpleCode::encode_next_hop(std::string dst)
{
	return Reply(5 + dst.size(), TYPE_NEXTHOP, dst);
}

Reply SimpleCode::encode_bandwith(float drop)
{
	string rate(to_string(drop));
	if(rate.size() > 10)
		rate = rate.substr(0, 10);
	return Reply(5 + rate.size(), TYPE_BANDWITH, rate);
}

Reply SimpleCode::encode_delay(float delay)
{
	string lay(to_string(delay));
	if(lay.size() > 10)
		lay = lay.substr(0, 10);
	return Reply(5 + lay.size(), TYPE_DELAY, lay);
}

Reply SimpleCode::encode_peer(std::string addr)
{
	return Reply(5 + addr.size(), TYPE_PEER, addr);
}

Reply SimpleCode::encode_error()
{
	return Reply(5, TYPE_ERROR, "");
}

LinkTask SimpleCode::decode_next_hop(unsigned char buff[], uint32_t len)
{
	char str[LEN+1];
	if(len-5 > LEN)
		return LinkTask();

	memset(str, 0, LEN+1);
	memcpy(str, buff+5, len-5);
	string dst(str);
	return LinkTask(-1, dst);
}

LinkTask SimpleCode::decode_bandwith(unsigned char buff[], uint32_t len)
{
	uint32_t bw;
	memset(&bw, 0, sizeof(bw));
	memcpy(&bw, buff+5, 4);

	char str[LEN+1];
	if(len > LEN)
		return LinkTask();

	memset(str, 0, LEN+1);
	memcpy(str, buff+9, len-9);
	string dst(str);
	return LinkTask(-1, dst, to_string(bw), "5");
}

LinkTask SimpleCode::decode_delay(unsigned char buff[], uint32_t len)
{
	char str[LEN+1];
	if(len-5 > LEN)
		return LinkTask();

	memset(str, 0, LEN+1);
	memcpy(str, buff+5, len-5);
	string dst(str);
	return LinkTask(-1, dst, "5");
}

LinkTask SimpleCode::decode_peer(unsigned char buff[], uint32_t len)
{
	char str[LEN];
	if(len-5 > LEN)
		return LinkTask();

	memset(str, 0, LEN+1);
	memcpy(str, buff+5, len-5);
	string dst(str);
	return LinkTask(-1);
}
