#pragma once
#include"INetMediator.h"
#include<iostream>
using namespace std;
class TcpClientMediator :public INetMediator
{
public:
	TcpClientMediator();
	~TcpClientMediator();
	bool OpenNet();
	void CloseNet();
	bool SendData(long ip, char* buf, int len);
	//�ײ���ϲ㷢�������ϲ㴦�� ��Ҫ֪����˭�����ģ������� �÷���ȥ
	void DealData(long ip, char* buf, int len);
};