#pragma once
class INet;
class INetMediator
{
public:
	INetMediator() {}
	~INetMediator() {}
	virtual bool OpenNet() = 0;
	virtual void CloseNet() = 0;
	virtual bool SendData(long ip, char* buf, int len) = 0;
	//�ײ���ϲ㷢�������ϲ㴦�� ��Ҫ֪����˭�����ģ������� �÷���ȥ
	virtual void DealData(long ip, char* buf, int len) = 0;
protected:
	INet* m_pNet;
};