#include"TcpServerMediator.h"
#include"TcpServer.h"
TcpServerMediator::TcpServerMediator()
{
	m_pNet = new TcpServer(this);
}

TcpServerMediator::~TcpServerMediator()
{
	CloseNet();
	if (m_pNet)
	{
		delete m_pNet;
		m_pNet = NULL;
	}
}

bool TcpServerMediator::OpenNet()
{
	if (m_pNet->InitNet())
		return true;
	return false;
}

void TcpServerMediator::CloseNet()
{
	m_pNet->UnInitNet();
}

bool TcpServerMediator::SendData(long ip, char* buf, int len)
{
	if (m_pNet->SendData(ip, buf, len))
		return true;
	return false;
}

void TcpServerMediator::DealData(long ip, char* buf, int len)
{
	//todo 打印接收到的数据
	cout << __func__ << ":" << buf << endl;
}
