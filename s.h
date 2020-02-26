#pragma once

#ifndef Myspider_H
#define	Myspider_H

#endif // !Myspider
#include<iostream>
#include<string>
#include<queue>
#include<Windows.h>
#include<fstream>
#include<vector>
#include<time.h>
#include<unordered_set>
#include<regex>

//#include<WinSock2.h>

#pragma comment(lib,"ws2_32.lib")

using namespace std;

class Myspider
{
public:
	Myspider() {
	};
	~Myspider() {
	};
	bool SockInit();   //�׽��ֳ�ʼ��
	bool SockConnect();//��������
	bool AnalyseURL(string url);//����url
	bool GetInfoHtml(string url); // ��ȡ��ҳ��Ϣ
	bool CatchHtml(string url);//��ȡurl
	bool download(string url, string filename); //����ͼƬ
	bool closeSock();   //�ر��׽���
	

private:

	string my_host;   //����
	string my_object;  //������
	SOCKET my_sock;  //�ͻ��׽���

};

