#pragma once
#include<iostream>
#include<WinSock2.h>
#include<map>
#include"packet.h"
using namespace std;
#pragma comment(lib,"Ws2_32.lib")
//����������࣬��ʾ������࣬�����Ų�ȥ�� һ��ͱ��뵽��
class INetMediator;
class INet
{
public: 
	INet(){}
	~INet(){}
	virtual bool InitNet() = 0;
	virtual void UnInitNet() = 0;
	virtual bool SendData(long ip,char*buf,int len) = 0;
protected:
	//ֻ��Ҫ���ڵ��õĺ������������óɱ�����
	virtual void RecvData() = 0;
	INetMediator* m_pMediator;
};