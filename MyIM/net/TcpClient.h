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
	//init时 构造函数时还无对象呢 要用静态成员函数去定义工作线程函数 不依赖对象
	static  unsigned __stdcall RecvThread(void* lpVoid);
	void RecvData();
	SOCKET m_sock;
	HANDLE m_handle;
	unsigned int m_threadId;
	bool m_isStop;
};