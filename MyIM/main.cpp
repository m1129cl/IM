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
		cout << "����� ��ʼ������ʧ��" << endl;
		return 1;
	}
	if (!pC->OpenNet())
	{
		cout << "�ͻ��� ��ʼ������ʧ��" << endl;
		return 1;
	}
	//�ͻ��˷�����
	pC->SendData(0, str, sizeof(str));
	while (1)
	{
		Sleep(10000);
		cout << "server is runing" << endl;
	}
	return 0;
}