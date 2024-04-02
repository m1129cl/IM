#include"TcpServer.h"
#include"TcpServerMediator.h"
#include<process.h>

TcpServer::TcpServer(INetMediator* mediator):m_sock(INVALID_SOCKET), m_isStop(false)
{
	m_pMediator = mediator;//new TcpServerMediator;

}

TcpServer::~TcpServer()
{
    UnInitNet();
}
//�����磺���ؿ� �����׽��� ��ip ����
bool TcpServer::InitNet()
{
    cout <<"tcpSever:"<< __func__ << endl;
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
    return true;
}
//���������̺߳���
unsigned __stdcall TcpServer::AcceptThread(void* lpVoid)
{
    TcpServer* pThis = (TcpServer*)lpVoid;//�÷����socket  ��̬����ʹ�� ��Ա����Ҫ�ж���
    sockaddr_in addrClient;
    int addrLen = sizeof(addrClient);
    unsigned int threadId = 0;
    while (!pThis->m_isStop)
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
    //�ر��׽���,�ǿ����ǺϷ���
    if (m_sock&&m_sock!=INVALID_SOCKET)closesocket(m_sock);
    for (map<unsigned int, SOCKET>::iterator ite = m_threadIdToSocket.begin(); ite != m_threadIdToSocket.end(); )
    {
        if(ite->second&&ite->second!=INVALID_SOCKET)
            closesocket(ite->second);
        //socket�ر��ˣ�����Ч���ˣ����Դ�map�Ƴ�
        ite = m_threadIdToSocket.erase(ite);
    }
    //�������պ������̸߳���ѱ�������ͽ�����
    m_isStop = true;
    //�ر��߳� ���߳̾�� 
    for (list<HANDLE>::iterator ite=m_handleList.begin();ite!=m_handleList.end();)
    {
        //��һ�ᣬ���̺߳���ִ�е�while(!m_isStop)������ѭ�� �Զ��˳�
        if ((*ite))
        {
            //��100���뻹û������ɱ��
            if(WaitForSingleObject((*ite),100)==WAIT_TIMEOUT)
                TerminateThread((*ite), 0);
            delete (*ite);
            (*ite) = NULL;
        }
        //����list��Ч�ڵ�
        ite = m_handleList.erase(ite);
    }
    
    //ж�ؿ�
    WSACleanup();
}
//������:long���� ���Լ���ip��ַ��socket
bool TcpServer::SendData(long ip, char* buf, int len)
{
    //�жϺϷ���
    if (buf == NULL ||len<=0)
    {
        cout <<__func__<< " TcpServer����ʧ�ܣ����ݰ��ǿյ� ���� len<=0\n" << endl;
        return false;
    }
    //�ȷ�����С
    if (send(ip, (char*)&len, sizeof(len), 0) <= 0)
    {
        cout << __func__ << "send error:" <<WSAGetLastError()<< endl;
        return false;
    }
    //cout << "Server send success!" << endl;
    //�ٷ�������
    if(send(ip, buf, len, 0)<=0)
    {
        cout << __func__ << "send error:" << WSAGetLastError() << endl;
        return false;
    }
    //cout << "Server send success!" << endl;
	return true;
}
//������
void TcpServer::RecvData()
{
    //���ӳɹ��� �����̣߳�����̴߳��������У�����id ��socket��û�ŵ�map���� ����Ȣ����socket
    //1������һ��
    Sleep(100);
    //2����ȡ��ǰ�̵߳� socket
    SOCKET sock = m_threadIdToSocket[GetCurrentThreadId()];
    if (!sock || sock == INVALID_SOCKET)
    {
        cout << __func__ << "socket error" << endl;
        return;
    }
    int recvLen = 0;
    int bufLen = 0;//���ڽ��հ��Ĵ�С
    sockaddr_in addr;
    int addrLen = sizeof(addr);
    char* buf = NULL;
    //��������Ҳ�������� ����ͻ���socket��Ҫͬʱ������ȥ�գ�����ÿһ���ͻ��˶�Ҫ��һ���߳�ȥ��������
    //���հ���С
    while (!m_isStop)
    {
        recvLen = recv(sock, (char*)&bufLen, sizeof(bufLen), 0);
        if (recvLen == 0)
        {
            //��������ʧ��
            cout << __func__ << "recv error:" << WSAGetLastError() << endl;
            return;
            //����ֵΪ0 ��ʾ�����ѶϿ�
            //�ر��߳� ��� socket
            //closesocket(sock);
            /*for (HANDLE handle : m_handleList)
            {
                if()
            }
            CloseHandle()*/
            //return;
        }
        else//���յ��İ���С����0
        {
            //֪��Ӧ���հ��Ĵ�С�����Ծ�new��ô��Ŀռ�
            /////mtu1460���ͻ���һ�η��Ŀ��ܰ��Ƚϴ����2000 ���Ի�ְ������Խ��վ���Ҫѭ�����ܣ��������ܽ���������
            buf = new char[bufLen];
            memset(buf, 0, bufLen);
            int offset = 0;//�洢�ۼƽ��յ�������

            //if(recv(sock, (char*)&buf, bufLen, 0)>0)
            while (bufLen > 0)
            {
                recvLen = recv(sock, (char*)buf + offset, bufLen, 0);
                if (recvLen > 0)
                {
                    //���յ�buf���ƫ�� ��bufʣ��ռ�Ĵ�С
                    offset += recvLen;//�ۼƽ���������
                    bufLen -= recvLen;//����Ҫ���յ�������
                }
                else
                {
                    //��������ʧ��
                    cout << __func__ << "recv error:" << WSAGetLastError() << endl;
                    return;
                }
            }
            //�������� �����н��� todo��������Ҫ��buf�����ռ����
            m_pMediator->DealData(sock, buf, offset);
        }
    }
    //�ٸ��ݰ���С �������������ݰ�
    //while (1)
    //{
    //    //recv(sock,)
    //}

}
//���������̺߳��� 
unsigned __stdcall TcpServer::RecvThread(void* lpVoid)
{
    TcpServer* pThis = (TcpServer*)lpVoid;
    pThis->RecvData();
    return 0;
}


