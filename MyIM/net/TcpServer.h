#pragma once
#include"INet.h"
#include<list>

//class TcpServerMediator;
class TcpServer :public INet
{
public:
	//���ﴫ���Ǹ����mediator����������ʶ�ģ�INet.h����INetMediator��������,��һ��д��TcpServerMediator�� �����
	TcpServer(INetMediator* mediator);
	~TcpServer();
	bool InitNet();
	void UnInitNet() ;
	bool SendData(long ip, char* buf, int len);
protected:
	void RecvData() ;
	//���������߳�,��̬-������������Ϳ���ʹ�� ��Ϊ�ڹ��캯���ﴴ���߳�ʱ��û�ж�����
	static  unsigned __stdcall AcceptThread(void* lpVoid);
	static  unsigned __stdcall RecvThread(void* lpVoid);
	SOCKET m_sock;
	//HANDLE m_handle;
	//�߳�id socket
	map<unsigned int, SOCKET>m_threadIdToSocket;
	//�����߳̾���������������ĸ��̵߳ľ�� ����Ϊֻ�ڹر�ʱʹ��
	list<HANDLE> m_handleList;
	//�߳��˳���־λ
	bool m_isStop;
};