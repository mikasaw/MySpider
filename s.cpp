#include "s.h"
#pragma warning(disable:4996)			    //用来取消fopen的不安全性警告


bool Myspider::SockInit()                           //套接字初始化
{
	WSADATA MyData;
	if (WSAStartup(MAKEWORD(2, 2), &MyData) != 0)
	{
		cout << "申请版本失败" << endl;
		return false;
	}
		

	if (LOBYTE(MyData.wVersion) != 2 || HIBYTE(MyData.wVersion) != 2)
	{
		cout << "版本错误" << endl;
		return false;
	}
		

	my_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (my_sock == INVALID_SOCKET)
	{
		cout << "创建套接字失败" << endl;
		return false;
	}

	return true;
}


 //连接网络
bool Myspider::SockConnect()			 
{
	if (SockInit() == false)
	{
		cout << "网络初始化失败" << endl;
		return false;
	}


	//解析IP地址
	hostent* host = gethostbyname(my_host.c_str());
	if (host == NULL)
	{
		cout << "解析IP地址失败" << endl;
		cout << h_errno << endl;
		return false;

	}

	//连接服务器
	sockaddr_in serverAdrr;
	serverAdrr.sin_family = AF_INET;
	serverAdrr.sin_port = htons(80);
	memcpy(&serverAdrr.sin_addr, host->h_addr, 4);

	if (SOCKET_ERROR == connect(my_sock, (sockaddr*)&serverAdrr, sizeof(serverAdrr)))
	{
		cout << "连接失败" << endl;
		return false;
	}

	return true;
}



//解析url
bool Myspider ::AnalyseURL(string url)
{
	if (url.empty())
	{
		cout << "解析的URL为空" << endl;
		return false;
	}
		

	if (url.length() <= 8)
	{
		cout << "URL不正确" << endl;
		return false;
	}
		
	if (url.find("http://") == string::npos) //找不到
	{
		cout << "找不到http" << endl;
		return false;
	}

	int pos = url.find('/', 7);				 //找到最后一个"/"的地址
	if (pos == string::npos)
	{
		my_host = url.substr(7);			// 从http;//后一位截到最后
		my_object = "/";
	}
	else
	{
		my_host = url.substr(7, pos - 8);  		//截取出网页的域名
		my_object = url.substr(pos);        		//截取出主机名
	}
	if (my_host.empty())
		return false;

	return true;
}


//获取网页信息
bool Myspider::GetInfoHtml(string url)
{
	string strInfo = "GET " + my_object + " HTTP/1.1\r\nHost:";
		strInfo = strInfo+ my_host + "\r\nConnection:close\r\n\r\n";    //编写请求的信息

	if (send(my_sock, strInfo.c_str(), strInfo.length(), 0) == SOCKET_ERROR)
	{
		cout << "发送失败" << endl;
		closesocket(my_sock);
		return false;
	}

	char re = 0;  			 //接收服务端发回的信息
	while (recv(my_sock,&re,1,0))
	{
		url = url + re;
	}
	closesocket(my_sock);
	return true;

}


//获取url
bool Myspider::CatchHtml(string url)
{
	vector<string> vImages;  	 	//存放图片的信息
	queue<string> InfoQueue;  		//存放信息的队列
	InfoQueue.push(url);

	while (!InfoQueue.empty())
	{
		string CurrentUrl = InfoQueue.front();
		InfoQueue.pop();
		Myspider http;
		http.AnalyseURL(CurrentUrl);
		cout << "主机名" << http.my_host << "\t资源路径" << http.my_object << endl;

		if (false == http.SockConnect())
		{
			cout << "连接失败" << endl;
			return false;
		}



		string html;
		http.GetInfoHtml(html);
		http.closeSock();


		//使用正则表达式来匹配所需要的数据  筛选出自己要爬取的类型
		smatch mat;
		regex rex("http://[^\\s'\"<>()]+");  			 // 在正则表达式里\s代表空白符
		string::const_iterator start = html.begin();
		string::const_iterator end = html.end();

		while (regex_search(start, end, mat, rex))
		{
			string per(mat[0].first, mat[0].second);

			if (per.find(".jpg") != string::npos || per.find(".png") != string::npos ||
				per.find(".jepg") != string::npos)
			{
				vImages.push_back(per);                //把是图片的url存储起来
			}
			else
			{
				if (per.find("http://www.w3.org/") == string::npos)
					InfoQueue.push(per);
			}
			start = mat[0].second;
		}


		//下载图片
		for (int i = 0; i < vImages.size(); i++)
		{
			string filename = "C:\\Users/Administrator/Desktop/Array/images/" +
				vImages[i].substr(vImages[i].find_last_of('/') + 1);

			Myspider down;
			if (down.download(vImages[i], filename) == 0)
				cout << "下载失败" << GetLastError() << endl;
			else
				cout << "下载成功" << endl;


		}

	}
	return true;
}



//下载图片函数
bool Myspider::download(string url, string filename)
{
	AnalyseURL(url);
	SockConnect();

	string RequestStr = "GET " + my_object + " HTTP/1.1\r\nHost:";
	RequestStr = RequestStr + my_host + "\r\nConnection:close\r\n\r\n";    //发送连接请求
	if (send(my_sock, RequestStr.c_str(), RequestStr.length(), 0) == SOCKET_ERROR)
	{
		cout << "发送失败" << endl;
		closesocket(my_sock);
		return false;
	}

	//打开文件准备接受数据
	FILE* fp = fopen(filename.c_str(), "w");
	if (fp == NULL)
	{
		cout << "打开文件失败" << endl;
		return false;
	}

	//过滤掉图片传回数据中的头部信息
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


//关闭套接字
bool Myspider::closeSock()
{
	closesocket(my_sock);
	my_object.clear();
	my_host.clear();
	my_sock = 0;
	return true;
}
