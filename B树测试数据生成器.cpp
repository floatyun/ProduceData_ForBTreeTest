// B树测试数据生成器.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <fstream>
#include <string>
#include <vector>
#include <exception>
#include <algorithm>
#include "student.h"
#include <atltime.h>
using namespace std;
string cmds[5] = {
	"insert",
	"find",
	"remove",
	"change",
	"exit"
};

int randint(int l, int r) {
	if (r - l != 0)
		return rand() % (r - l) + l;
	else
		return l;
}

void randAStudent(student&x) {
	x.name.clear();
	for (int i = 0; i < 3; ++i)
		x.name.push_back(char(randint('a','z'+1)));
	x.score = randint(0,750);
}

struct Stus {
	vector<student>datas;
	void insert(const student&key);
	void remove(const student&key);
	student* find(const student&key);
	void randANameFromDatas(string&s);
}stus;

// 返回时间差left-right，以毫秒计数
uint64_t TimeDiff(SYSTEMTIME &left, SYSTEMTIME &right)

{
	CTime tmLeft(left.wYear, left.wMonth, left.wDay, 0, 0, 0);
	CTime tmRight(right.wYear, right.wMonth, right.wDay, 0, 0, 0);
	CTimeSpan sp = tmLeft - tmRight;
	long MillisecondsL = (left.wHour * 3600 + left.wMinute * 60 + left.wSecond) * 1000 + left.wMilliseconds;
	long MillisecondsR = (right.wHour * 3600 + right.wMinute * 60 + right.wSecond) * 1000 + right.wMilliseconds;
	return  (uint64_t)sp.GetDays() * 86400000 + (MillisecondsL - MillisecondsR);
}

int main()
{
	srand(time(NULL));
	student tmp;
	student *res;
	student t;
	//int count = randint(12,20);
	int count;
	cout << "输入命令条数";
	cin >> count;
	ofstream inputfile("input.txt");
	ofstream outputfile("output.txt");
	int isFromExist;
	SYSTEMTIME  before, after;

	GetSystemTime(&before);
	for (int i = 0; i < count; ++i) {
		//cout << i << endl;
		int j = randint(0, 4);
		if (i * 20 < count) // 大概前面5%的命令都是插入命令
			j = 0;
		//cout << "j = " << j << endl;
		string cmd = cmds[j];

		isFromExist = randint(0, 100);
		
		try {
			if ("exit" == cmd || "quit" == cmd) {
				inputfile << "exit" << endl;
				break;
			}
			else if ("insert" == cmd || "add" == cmd) {
				randAStudent(tmp);
				if (isFromExist < 10) { 
					// 90%的几率是随机产生名字（很小的可能产生的名字已经存在）
					// 10%的几率（0-19）是从已有的名字中随机产生一个名字
					stus.randANameFromDatas(tmp.name);
				}

				inputfile << "insert " << tmp << endl;

				stus.insert(tmp);
				outputfile << tmp << "已经成功插入" << endl;
			}
			else if ("remove" == cmd || "delete" == cmd) {
				randAStudent(tmp);
				if (isFromExist < 90) {
					// 10%的几率是随机产生名字（很小的可能产生的名字已经存在）
					// 90%的几率（0-79）是从已有的名字中随机产生一个名字
					stus.randANameFromDatas(tmp.name);
				}

				inputfile << "remove " << tmp.name << endl;

				stus.remove(tmp);
				outputfile << "已经成功删除" << tmp.name << endl;
			}
			else if ("find" == cmd || "query" == cmd) {
				randAStudent(tmp);
				if (isFromExist < 90) {
					// 10%的几率是随机产生名字（很小的可能产生的名字已经存在）
					// 90%的几率（0-79）是从已有的名字中随机产生一个名字
					stus.randANameFromDatas(tmp.name);
				}

				inputfile << "find " << tmp.name << endl;

				res = stus.find(tmp);
				if (nullptr == res)
					outputfile << tmp.name << "不存在" << endl;
				else
					outputfile << "已成功找到" << *res << endl;
			}
			else if ("change" == cmd) {
				randAStudent(tmp);
				if (isFromExist < 90) {
					// 10%的几率是随机产生名字（很小的可能产生的名字已经存在）
					// 90%的几率（0-79）是从已有的名字中随机产生一个名字
					stus.randANameFromDatas(tmp.name);
				}

				inputfile << "change " << tmp << endl;

				res = stus.find(tmp);
				if (nullptr == res) {
					outputfile << tmp.name << "不存在，";
					outputfile << "将直接插入这条记录。" << tmp;
					stus.insert(tmp);
					outputfile << "已经成功插入" << endl;
				}
				else {
					t = *res;
					stus.remove(t);
					stus.insert(tmp);
					outputfile << "已成功将" << tmp.name << "的成绩由" << t.score << "修改为" << tmp.score << endl;
				}
			}
			else {
				outputfile << cmd << "命令不存在" << endl;
			}
		}
		catch (exception e) {
			outputfile << e.what() << endl;
		}
	}
	inputfile << "exit" << endl;
	GetSystemTime(&after);
	cout << TimeDiff(after, before) << "ms\n";
	inputfile.close();
	outputfile.close();
	return 0;
}

void Stus::insert(const student & key)
{
	student_printer printer;
	if (nullptr != find(key))
		throw std::invalid_argument(printer.to_string(key) + "已存在，无需插入 ");
	auto it = upper_bound(datas.begin(), datas.end(), key);
	datas.insert(it, key);
}

void Stus::remove(const student & key)
{
	student_printer printer;
	auto it = std::find(datas.begin(), datas.end(), key);
	if (it == datas.end())
		throw std::invalid_argument(printer.to_string(key) + "不存在，无需删除 ");
	it = datas.erase(it);
}

student * Stus::find(const student & key)
{
	auto it = std::find(datas.begin(), datas.end(), key);
	if (it == datas.end())
		return nullptr;
	else
		return &(*it);
}

void Stus::randANameFromDatas(string & s)
{
	if (datas.size() < 2) return;
	s = datas[randint(0, datas.size())].name;
}
