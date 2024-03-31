#include"TcpClient.h"
TcpClient::TcpClient(INetMediator* mediator)
{
	m_pMediator = mediator;
}

TcpClient::~TcpClient()
{
}

bool TcpClient::InitNet()
{
	return false;
}

void TcpClient::UnInitNet()
{
}

bool TcpClient::SendData(long ip, char* buf, int len)
{
	return false;
}

void TcpClient::RecvData()
{
}
