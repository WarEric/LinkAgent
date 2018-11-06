/**
 * Some function's defination of SimpleCode class.
 *
 * by wareric@163.com
 * 2018-11-02
 */
#include <cstring>
#include <string.h>
#include "simplecode.h"
#define MAXLINE		4096

using std::string;
using std::to_string;

LinkTask SimpleCode::decode(char buff[])
{
	int len, type;
	memset(&len, 0, sizeof(len));
	memset(&type, 0, sizeof(type));

	char *ptr = buff;
	memcpy(&len, ptr, 4);
	memcpy(&type, ptr+4, 1);

	switch(type){
		case TYPE_NEXTHOP:
			return decode_next_hop(buff, len);
		case TYPE_BANDWIDTH:
			return decode_bandwith(buff, len);
		case TYPE_DELAY:
			return decode_bandwith(buff, len);
		case TYPE_PEER:
			return decode_peer(buff, len);
		default:
			return LinkTask();
	}
	return LinkTask();
}

std::string SimpleCode::encode_next_hop(std::string dst)
{
	char buff[MAXLINE];
	short len = 5 + dst.size();
	memset(buff, 0, MAXLINE);

	memcpy(buff, &len, 4);
	buff[4] = TYPE_NEXTHOP;
	memcpy(buff+5, dst.c_str(), dst.size());

	return string(buff);
}

std::string SimpleCode::encode_bandwith(float drop)
{
	string rate(to_string(drop));
	if(rate.size() > 10)
		rate = rate.substr(0, 10);
	short len = 5 + rate.size();
	char buff[MAXLINE];
	memset(buff, 0, MAXLINE);

	memcpy(buff, &len, 4);
	buff[4] =  TYPE_BANDWIDTH;
	memcpy(buff+5, rate.c_str(), rate.size());

	return string(buff);
}

std::string SimpleCode::encode_delay(float delay)
{
	string lay(to_string(delay));
	if(lay.size() > 10)
		lay = lay.substr(0, 10);
	short len = 5 + lay.size();
	char buff[MAXLINE];
	memset(buff, 0, MAXLINE);

	memcpy(buff, &len, 4);
	buff[4] = TYPE_DELAY;
	memcpy(buff+5, lay.c_str(), lay.size());

	return string(buff);
}

std::string SimpleCode::encode_peer(std::string addr)
{
	char buff[MAXLINE];
	short len = 5 + addr.size();
	memset(buff, 0, MAXLINE);

	memcpy(buff, &len, 4);
	buff[4] = TYPE_PEER;
	memcpy(buff+5, addr.c_str(), addr.size());

	return string(buff);
}

std::string SimpleCode::encode_error()
{
	short len = 5;
	char buff[MAXLINE];
	memset(buff, 0, MAXLINE);

	memcpy(buff, &len, 4);
	buff[4] = TYPE_ERROR;

	return string(buff);
}

LinkTask SimpleCode::decode_next_hop(char buff[], int len)
{
	char str[MAXLINE];
	if(len-5 > MAXLINE-1)
		return LinkTask();

	memcpy(str, buff+5, len-5);
	str[len-5] = 0;
	std::string dst(str);
	return LinkTask(-1, dst);
}

LinkTask SimpleCode::decode_bandwith(char buff[], int len)
{
	int bw;
	memset(&bw, 0, sizeof(bw));
	memcpy(&bw, buff+5, 4);

	char str[MAXLINE];
	if(len-9 > MAXLINE-1)
		return LinkTask();

	memcpy(str, buff+9, len-9);
	str[len-9] = 0;
	std::string dst(str);
	return LinkTask(-1, dst, to_string(bw), "5");
}

LinkTask SimpleCode::decode_delay(char buff[], int len)
{
	char str[MAXLINE];
	if(len-5 > MAXLINE-1)
		return LinkTask();

	memcpy(str, buff+5, len-5);
	str[len-5] = 0;
	std::string dst(str);
	return LinkTask(-1, dst, "5");
}

LinkTask SimpleCode::decode_peer(char buff[], int len)
{
	char str[MAXLINE];
	if(len-5 > MAXLINE-1)
		return LinkTask();

	memcpy(str, buff+5, len-5);
	str[len-5] = 0;
	std::string dst(str);
	return LinkTask(-1);
}
