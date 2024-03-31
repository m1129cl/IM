#include"TcpServer.h"
#include"TcpServerMediator.h"
#include<process.h>

TcpServer::TcpServer(INetMediator* mediator):m_sock(INVALID_SOCKET)
{
	m_pMediator = mediator;//new TcpServerMediator;
}

TcpServer::~TcpServer()
{
}
//�����磺���ؿ� �����׽��� ��ip ����
bool TcpServer::InitNet()
{
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
    //3.��ip��ַ
    sockaddr_in addrServer;
    addrServer.sin_family = AF_INET;
    addrServer.sin_port = htons(_DEF_TCP_PORT);//�������ֽ���С�ˣ�ת���������ֽ��򣨴�ˣ�
    addrServer.sin_addr.S_un.S_addr = INADDR_ANY;//����������
    err = bind(m_sock, (sockaddr*)&addrServer, sizeof(addrServer));
    if (SOCKET_ERROR == err)
    {
        cout << "bind err" << WSAGetLastError() << endl;
        UnInitNet();
        return false;
    }
    else
    {
        cout << "bind success" << endl;
    }

    //4.����
    err = listen(m_sock, 10);
    if (SOCKET_ERROR == err)
    {
        cout << "listen error:" << WSAGetLastError() << endl;
        UnInitNet();
        return false;
    }
    else
    {
        cout << "listen success" << endl;
    }
    //�������ӣ��������Ӷ���ͻ��˵����� ������һֱ�������� ��ִ��һ������Ҫ�ö��̣߳�
    //ʹ�� _beginthreadex��>endthreadex�Ȼ��տռ� ��ȥ����existthread ;�����Զ�����C++����ʱ�����Դ createthread��Ҫ�����ֶ���������
    HANDLE handle =(HANDLE) _beginthreadex(0, 0, &AcceptThread, this, 0, NULL);//��־λ0�����������У�thradId
    if (handle)
        m_handleList.push_back(handle);
    return false;
}
//���������̺߳���
unsigned __stdcall TcpServer::AcceptThread(void* lpVoid)
{
    TcpServer* pThis = (TcpServer*)lpVoid;//�÷����socket  ��̬����ʹ�� ��Ա����Ҫ�ж���
    sockaddr_in addrClient;
    int addrLen = sizeof(addrClient);
    unsigned int threadId = 0;
    while (1)
    {
        //��������
        ///��̬��Ա�������ܷ��� �Ǿ�̬��Ա����̬�����ڲ�Ҫ�ж����ָ��ȥ����
        SOCKET sock = accept(pThis->m_sock, (sockaddr*)&addrClient,&addrLen);
        cout << "accpet success,ip:" << inet_ntoa(addrClient.sin_addr) << endl;
        //�����ӳɹ��Ŀͻ��� Ҫ����һ���������ݵ��߳�
        //�߳̾�� ���ڽ����߳� ���ջ����ں˶���ʱʹ��
        HANDLE handle = (HANDLE)_beginthreadex(0, 0, &RecvThread, pThis, 0, &threadId);//��־λ0�����������У�thradId
        if(handle)
            pThis->m_handleList.push_back(handle);
        //���߳�id ��socket���浽map��
        pThis->m_threadIdToSocket[threadId] = sock;
    }
    return 0;
}

//�ر�����
void TcpServer::UnInitNet()
{
}
//������
bool TcpServer::SendData(long ip, char* buf, int len)
{
    //�жϺϷ���
    //�ܷ�����С
    //�ٷ�������
    send(ip, buf, len, 0);
	return false;
}
//������
void TcpServer::RecvData()
{
    SOCKET sock = m_threadIdToSocket[GetCurrentThreadId()];
    //��������Ҳ�������� ����ͻ���socket��Ҫͬʱ������ȥ�գ�����ÿһ���ͻ��˶�Ҫ��һ���߳�ȥ��������
    while (1)
    {
        //recv(sock,)
    }

}
//���������̺߳��� 
unsigned __stdcall TcpServer::RecvThread(void* lpVoid)
{
    TcpServer* pThis = (TcpServer*)lpVoid;
    pThis->RecvData();
    return 0;
}


