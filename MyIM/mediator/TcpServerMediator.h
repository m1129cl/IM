#pragma once
#include"INetMediator.h"
#include<iostream>
using namespace std;
class TcpServerMediator:public INetMediator
{
public:
	TcpServerMediator();
	~TcpServerMediator();
	bool OpenNet();
	void CloseNet() ;
	bool SendData(long ip, char* buf, int len);
	//�ײ���ϲ㷢�������ϲ㴦�� ��Ҫ֪����˭�����ģ������� �÷���ȥ
	void DealData(long ip, char* buf, int len);
};