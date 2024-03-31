#pragma once
#include<iostream>
#include<WinSock2.h>
#include<map>
#include"packet.h"
using namespace std;
#pragma comment(lib,"Ws2_32.lib")
//先声明这个类，表示有这个类，先用着不去找 一会就编译到了
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
	//只需要类内调用的函数，所以设置成保护的
	virtual void RecvData() = 0;
	INetMediator* m_pMediator;
};