#include"TcpClient.h"
#include"packet.h"
#include"TcpClientMediator.h"
#include<process.h>
TcpClient::TcpClient(INetMediator* mediator):m_sock(INVALID_SOCKET), m_threadId(0),m_handle(NULL), m_isStop(false)
{
	m_pMediator = mediator;
}

TcpClient::~TcpClient()
{
    UnInitNet();
}

bool TcpClient::InitNet()
{
    cout <<"tcpClient:" << __func__ << endl;
    //1.���ؿ�
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;

    /* Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h */
    wVersionRequested = MAKEWORD(2, 2);

    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {
        /* Tell the user that we could not find a usable */
        /* Winsock DLL.                                  */
        printf("WSAStartup failed with error: %d\n", err);
        return false;
    }

    /* Confirm that the WinSock DLL supports 2.2.*/
    /* Note that if the DLL supports versions greater    */
    /* than 2.2 in addition to 2.2, it will still return */
    /* 2.2 in wVersion since that is the version we      */
    /* requested.                                        */

    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
        /* Tell the user that we could not find a usable */
        /* WinSock DLL.                                  */
        printf("Could not find a usable version of Winsock.dll\n");
        UnInitNet();
        return false;
    }
    else
    {
        printf("The Winsock 2.2 dll was found okay\n");
    }

    //2.�����׽���
    m_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_sock == INVALID_SOCKET)
    {
        cout << "socket error:" << WSAGetLastError() << endl;
        UnInitNet();
        return false;
    }
    else
    {
        cout << "socket success" << endl;
    }
    //3�����ӷ����
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(_DEF_TCP_PORT);
    addr.sin_addr.S_un.S_addr = inet_addr(_DEF_TCP_IP);
    
    //�ɹ�����0
    int connectReturn = 0;
    if ((connectReturn=connect(m_sock, (sockaddr*)&addr, sizeof(addr)))!=0)//���ӳ���
    {
        cout << "TcpClient connect ����ʧ��:" <<WSAGetLastError()<< endl;
        return false;
    }
    else
    {
        cout << "connect success" << endl;
    }
    //4��ѭ���շ����� ��������Ҫ�����µ��߳�,ֻ���շ���˵�����
    //unsigned int ThreadId = 0;
    m_handle = (HANDLE)_beginthreadex(0, 0, &RecvThread, this, 0, 0);//�����ڶ�������0������������
	return true;
}
unsigned __stdcall TcpClient::RecvThread(void* lpVoid)
{
    TcpClient* pThis = (TcpClient*)lpVoid;
    pThis->RecvData();
    return 0;
}
void TcpClient::UnInitNet()
{
    //�ر��׽���
    if (m_sock != INVALID_SOCKET)closesocket(m_sock);
    //�ر��߳� ���߳̾�� and SOCKET
    m_isStop = true;
    if (m_handle)
    {
        if (WaitForSingleObject(m_handle, 100) == WAIT_TIMEOUT)
        {
            TerminateThread(m_handle, 0);
        }
        delete m_handle;
        m_handle = NULL;
        //CloseHandle(m_handle); ��֪��Ϊɶ��ͨ�����
    }  
    
    //ж�ؿ�
    WSACleanup();
}

bool TcpClient::SendData(long ip, char* buf, int len)
{
    //�жϺϷ���
    if (buf == NULL||len<=0)
    {
        cout << "TcpServer����ʧ�ܣ����ݰ��ǿյ�\n" << endl;
        return false;
    }
    //if (ip == NULL)//�ͻ��˷��Ϳ�����գ���Ϊ��ʼ������ʱ �Ѿ������������sock�� ���ͻ���Ҳֻ�ͷ���˽���
    //    ip = m_sock;
    //�ȷ�����С

    if(send(m_sock, (char*)&len, sizeof(len), 0)<=0)
    {
        cout << __func__ << "����ʧ��" << endl;
        return false;
    }
    //�ٷ�������
    if (send(m_sock, buf, len, 0) <= 0)
    {
        cout << __func__ << "����ʧ��" << endl;
        return false;
    }
	return true;
}

void TcpClient::RecvData()
{
    //cout << "TcpClient::RecvData()" << endl;
    
    int recvLen = 0;
    int bufLen = 0;//���ڽ��հ��Ĵ�С
    sockaddr_in addr;
    int addrLen = sizeof(addr);
    char* buf=NULL;
    //��������Ҳ�������� ����ͻ���socket��Ҫͬʱ������ȥ�գ�����ÿһ���ͻ��˶�Ҫ��һ���߳�ȥ��������
    //���հ���С
    while (!m_isStop)
    {
        //cout << "ing1 " << endl;
        recvLen = recv(m_sock, (char*)&bufLen, sizeof(bufLen), 0);
        //cout << "ing2 " << endl;
        if (recvLen == 0)
        {
            cout << "recvData:�ͷ���������ѶϿ�,�رտͻ���" << endl;
            UnInitNet();
            return;
        }
        else if(recvLen>0)//���յ��İ���С����0
        {
            //cout << "recvLen>0" << endl;
            buf = new char[bufLen];
            int offset = 0;
            while (bufLen > 0)//ֻҪ���ݻ���û��ȡ�ģ���û��ȡ��recv�Ͳ��Ῠ�������ͽ��Ŷ�
            {
                recvLen = recv(m_sock, (char*)buf + offset, bufLen, 0);
                
                //���յ�buf���ƫ�� ��bufʣ��ռ�Ĵ�С
                offset += recvLen;
                bufLen -= recvLen;
            }
            //�������� �����н��� todo��������Ҫ��buf�����ռ����
            m_pMediator->DealData(m_sock, buf, offset);
        }
        //cout << "ing3" << endl;
    }
    //cout << "ing4 " << endl;
}
