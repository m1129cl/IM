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
	//底层给上层发来，让上层处理 需要知道是谁发来的，处理完 好发回去
	virtual void DealData(long ip, char* buf, int len) = 0;
protected:
	INet* m_pNet;
};