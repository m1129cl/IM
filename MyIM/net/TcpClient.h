#pragma once
#include"INet.h"
//class TcpServerMediator;
class TcpClient :public INet
{
public:
	//���ﴫ���Ǹ����mediator����������ʶ�ģ�INet.h����INetMediator��������,��һ��д��TcpServerMediator�� �����
	TcpClient(INetMediator* mediator);
	~TcpClient();
	bool InitNet();
	void UnInitNet();
	bool SendData(long ip, char* buf, int len);
protected:
	void RecvData();
};