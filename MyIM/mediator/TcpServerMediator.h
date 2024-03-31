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
	//底层给上层发来，让上层处理 需要知道是谁发来的，处理完 好发回去
	void DealData(long ip, char* buf, int len);
};