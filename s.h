#pragma once

#ifndef Myspider_H
#define	Myspider_H


#include<iostream>
#include<string>
#include<queue>
#include<Windows.h>
#include<fstream>
#include<vector>
#include<time.h>
#include<regex>


#pragma comment(lib,"ws2_32.lib")

using namespace std;

class Myspider
{
public:
	Myspider() {
	};
	~Myspider() {
	};
	bool SockInit();   //套接字初始化
	bool SockConnect();//连接网络
	bool AnalyseURL(string url);//解析url
	bool GetInfoHtml(string url); // 获取网页信息
	bool CatchHtml(string url);//获取url
	bool download(string url, string filename); //下载图片
	bool closeSock();   //关闭套接字
	

private:

	string my_host;   //域名
	string my_object;  //主机名
	SOCKET my_sock;  //客户套接字

};

#endif // !Myspider
