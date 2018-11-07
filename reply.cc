/**
 * by wareric@163.com
 * 2018-11-07
 **/
#include <string>
#include <string.h>
#include "reply.h"
using std::string;

Reply::Reply(uint32_t length, uint8_t t, string message):msg(message)
{
	len = length;
	type = t;
}

bool Reply::encode(void* buff, int maxlen)
{
	char *ptr = (char *)buff;
	if((int)len > maxlen)
		return false;
	memcpy(ptr, &len, 4); 
	memcpy(ptr+4, &type, 1); 
	memcpy(ptr+5, msg.c_str(), msg.size());
	return true;
}   

bool Reply::decode(Reply &dst, void* buff, int maxlen)
{
	if(maxlen < 5)
		return false;

	char *ptr = (char *)buff;
	uint32_t _len;
	memcpy(&_len, ptr, 4);
	dst.len = _len;

	uint8_t _type;
	memcpy(&_type, ptr+4, 1);
	dst.type = _type;

	if((int)_len > maxlen)
		return false;

	char str[MAXLINE];
	memset(str, 0, MAXLINE);
	memcpy(str, ptr+5, _len-5);
	dst.msg = string(str);
	return true;
}
