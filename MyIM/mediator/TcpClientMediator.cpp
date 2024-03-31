#include"TcpClientMediator.h"
#include"TcpClient.h"
TcpClientMediator::TcpClientMediator()
{
	m_pNet = new TcpClient(this);
}

TcpClientMediator::~TcpClientMediator()
{
	CloseNet();
	if (m_pNet)
	{
		delete m_pNet;
		m_pNet = NULL;
	}
}

bool TcpClientMediator::OpenNet()
{
	if (m_pNet->InitNet())
		return true;
	return false;
}

void TcpClientMediator::CloseNet()
{
	m_pNet->UnInitNet();
}

bool TcpClientMediator::SendData(long ip, char* buf, int len)
{
	if (m_pNet->SendData(ip, buf, len))
		return true;
	return false;
}

void TcpClientMediator::DealData(long ip, char* buf, int len)
{
	//todo 打印接收到的数据
	cout << __func__ << ":" << buf << endl;
}
