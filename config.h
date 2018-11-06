/**
 * 1. read infomation from config file in the format of map;
 * 2. line start with charater '#' will be regard as comment
 * line, datas are recorded in the format of "key = value ", it
 * doesn't matter whether it has blank space between words.
 *
 * by wareric@163.com
 * 2018-11-06
 * */
 
#ifndef SHADOW_H_H
#define SHADOW_H_H
#define COMMENT_CHAR '#'
#include <string>
#include <map>
#include <set>
 
bool readfile(const std::string &filename, std::map<std::string, std::string> &m);
bool readfile(const std::string &filename, std::set<std::string> &m);
#endif
