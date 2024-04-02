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
	//initʱ ���캯��ʱ���޶����� Ҫ�þ�̬��Ա����ȥ���幤���̺߳��� ����������
	static  unsigned __stdcall RecvThread(void* lpVoid);
	void RecvData();
	SOCKET m_sock;
	HANDLE m_handle;
	unsigned int m_threadId;
	bool m_isStop;
};