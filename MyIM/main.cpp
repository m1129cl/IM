#include<iostream>

#include"CMySql.h"
#include"TcpServerMediator.h"
#include"TcpClientMediator.h"
using namespace std;
int main()
{
	CMySql mysql;
	char str[10] = "hellow\0";
	INetMediator* pS = new TcpServerMediator;
	INetMediator* pC = new TcpClientMediator;
	if (!pS->OpenNet())
	{
		cout << "服务端 初始化网络失败" << endl;
		return 1;
	}
	if (!pC->OpenNet())
	{
		cout << "客户端 初始化网络失败" << endl;
		return 1;
	}
	//客户端发数据
	pC->SendData(0, str, sizeof(str));
	while (1)
	{
		Sleep(10000);
		cout << "server is runing" << endl;
	}
	return 0;
}