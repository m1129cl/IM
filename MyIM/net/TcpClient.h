#pragma once
#include"INet.h"
//class TcpServerMediator;
class TcpClient :public INet
{
public:
	//这里传参是父类的mediator（这样是认识的，INet.h里有INetMediator的声明）,第一次写成TcpServerMediator了 错误的
	TcpClient(INetMediator* mediator);
	~TcpClient();
	bool InitNet();
	void UnInitNet();
	bool SendData(long ip, char* buf, int len);
protected:
	void RecvData();
};