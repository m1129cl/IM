#pragma once
#include"INet.h"
#include<list>

//class TcpServerMediator;
class TcpServer :public INet
{
public:
	//这里传参是父类的mediator（这样是认识的，INet.h里有INetMediator的声明）,第一次写成TcpServerMediator了 错误的
	TcpServer(INetMediator* mediator);
	~TcpServer();
	bool InitNet();
	void UnInitNet() ;
	bool SendData(long ip, char* buf, int len);
protected:
	void RecvData() ;
	//接受链接线程,静态-》不依赖对象就可以使用 因为在构造函数里创建线程时还没有对象呢
	static  unsigned __stdcall AcceptThread(void* lpVoid);
	static  unsigned __stdcall RecvThread(void* lpVoid);
	SOCKET m_sock;
	//HANDLE m_handle;
	//线程id socket
	map<unsigned int, SOCKET>m_threadIdToSocket;
	//保存线程句柄，不用区分是哪个线程的句柄 ，因为只在关闭时使用
	list<HANDLE> m_handleList;
	//线程退出标志位
	bool m_isStop;
};