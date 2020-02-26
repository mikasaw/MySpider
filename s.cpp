#include "s.h"
#pragma warning(disable:4996)


bool Myspider::SockInit()   //�׽��ֳ�ʼ��
{
	WSADATA MyData;
	if (WSAStartup(MAKEWORD(2, 2), &MyData) != 0)
	{
		cout << "����汾ʧ��" << endl;
		return false;
	}
		

	if (LOBYTE(MyData.wVersion) != 2 || HIBYTE(MyData.wVersion) != 2)
	{
		cout << "�汾����" << endl;
		return false;
	}
		

	my_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (my_sock == INVALID_SOCKET)
	{
		cout << "�����׽���ʧ��" << endl;
		return false;
	}

	return true;
}



bool Myspider::SockConnect()//��������
{
	if (SockInit() == false)
	{
		cout << "�����ʼ��ʧ��" << endl;
		return false;
	}


	//����IP��ַ
	hostent* host = gethostbyname(my_host.c_str());
	if (host == NULL)
	{
		cout << "����IP��ַʧ��" << endl;
		cout << h_errno << endl;
		return false;

	}

	//���ӷ�����
	sockaddr_in serverAdrr;
	serverAdrr.sin_family = AF_INET;
	serverAdrr.sin_port = htons(80);
	memcpy(&serverAdrr.sin_addr, host->h_addr, 4);

	if (SOCKET_ERROR == connect(my_sock, (sockaddr*)&serverAdrr, sizeof(serverAdrr)))
	{
		cout << "����ʧ��" << endl;
		return false;
	}

	return true;
}


//����url
bool Myspider ::AnalyseURL(string url)
{
	if (url.empty())
	{
		cout << "������URLΪ��" << endl;
		return false;
	}
		

	if (url.length() <= 8)
	{
		cout << "URL����ȷ" << endl;
		return false;
	}
		
	if (url.find("http://") == string::npos) //�Ҳ���
	{
		cout << "�Ҳ���http" << endl;
		return false;
	}

	int pos = url.find('/', 7);				 //�ҵ����һ��"/"�ĵ�ַ
	if (pos == string::npos)
	{
		my_host = url.substr(7);			// ��http;//��һλ�ص����
		my_object = "/";
	}
	else
	{
		my_host = url.substr(7, pos - 8);   //��ȡ����ҳ������
		my_object = url.substr(pos);        //��ȡ��������
	}
	if (my_host.empty())
		return false;

	return true;
}


//��ȡ��ҳ��Ϣ
bool Myspider::GetInfoHtml(string url)
{
	string strInfo = "GET " + my_object + " HTTP/1.1\r\nHost:";
		strInfo = strInfo+ my_host + "\r\nConnection:close\r\n\r\n";   //��д�������Ϣ

	if (send(my_sock, strInfo.c_str(), strInfo.length(), 0) == SOCKET_ERROR)
	{
		cout << "����ʧ��" << endl;
		closesocket(my_sock);
		return false;
	}

	char re = 0;   //���շ���˷��ص���Ϣ
	while (recv(my_sock,&re,1,0))
	{
		url = url + re;
	}
	closesocket(my_sock);
	return true;

}


//��ȡurl
bool Myspider::CatchHtml(string url)
{
	vector<string> vImages;   //���ͼƬ����Ϣ
	queue<string> InfoQueue;  //�����Ϣ�Ķ���
	InfoQueue.push(url);

	while (!InfoQueue.empty())
	{
		string CurrentUrl = InfoQueue.front();
		InfoQueue.pop();
		Myspider http;
		http.AnalyseURL(CurrentUrl);
		cout << "������" << http.my_host << "\t��Դ·��" << http.my_object << endl;

		if (false == http.SockConnect())
		{
			cout << "����ʧ��" << endl;
			return false;
		}



		string html;
		http.GetInfoHtml(html);
		http.closeSock();


		//ʹ��������ʽ��ƥ������Ҫ������
		smatch mat;
		regex rex("http://[^\\s'\"<>()]+");   // ��������ʽ��\s����հ׷�
		string::const_iterator start = html.begin();
		string::const_iterator end = html.end();

		while (regex_search(start, end, mat, rex))
		{
			string per(mat[0].first, mat[0].second);

			if (per.find(".jpg") != string::npos || per.find(".png") != string::npos ||
				per.find(".jepg") != string::npos)
			{
				vImages.push_back(per);                //����ͼƬ��url�洢����
			}
			else
			{
				if (per.find("http://www.w3.org/") == string::npos)
					InfoQueue.push(per);
			}
			start = mat[0].second;
		}


		//����ͼƬ
		for (int i = 0; i < vImages.size(); i++)
		{
			string filename = "C:\\Users/Administrator/Desktop/Array/images/" +
				vImages[i].substr(vImages[i].find_last_of('/') + 1);

			Myspider down;
			if (down.download(vImages[i], filename) == 0)
				cout << "����ʧ��" << GetLastError() << endl;
			else
				cout << "���سɹ�" << endl;


		}

	}
	return true;
}



//����ͼƬ����
bool Myspider::download(string url, string filename)
{
	AnalyseURL(url);
	SockConnect();

	string RequestStr = "GET " + my_object + " HTTP/1.1\r\nHost:";
	RequestStr = RequestStr + my_host + "\r\nConnection:close\r\n\r\n";    //������������
	if (send(my_sock, RequestStr.c_str(), RequestStr.length(), 0) == SOCKET_ERROR)
	{
		cout << "����ʧ��" << endl;
		closesocket(my_sock);
		return false;
	}

	//���ļ�׼����������
	FILE* fp = fopen(filename.c_str(), "w");
	if (fp == NULL)
	{
		cout << "���ļ�ʧ��" << endl;
		return false;
	}

	//���˵�ͼƬ���������е�ͷ����Ϣ
	char ch1 = 0;
	while (recv(my_sock, &ch1, 1, 0))
	{
		if (ch1 == '\r')
		{
			{ 
				recv(my_sock, &ch1, 1, 0);
			   if (ch1 == '\n')
			   {
				  recv(my_sock, &ch1, 1, 0);
					 if (ch1 == '\r')
					 {
						recv(my_sock, &ch1, 1, 0);
						 if (ch1 == '\n')
							break;
					 }
			   }
			}
		}

	}

	char ch2 = 0;
	while (recv(my_sock,&ch2,1,0))
	{
		fwrite(&ch2, 1, 1, fp);
	}
	
	fclose(fp);
	return true;

}


//�ر��׽���
bool Myspider::closeSock()
{
	closesocket(my_sock);
	my_object.clear();
	my_host.clear();
	my_sock = 0;
	return true;
}
