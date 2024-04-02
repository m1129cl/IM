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
    //1.加载库
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

    //2.创建套接字
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
    //3、连接服务端
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(_DEF_TCP_PORT);
    addr.sin_addr.S_un.S_addr = inet_addr(_DEF_TCP_IP);
    
    //成功返回0
    int connectReturn = 0;
    if ((connectReturn=connect(m_sock, (sockaddr*)&addr, sizeof(addr)))!=0)//连接出错
    {
        cout << "TcpClient connect 连接失败:" <<WSAGetLastError()<< endl;
        return false;
    }
    else
    {
        cout << "connect success" << endl;
    }
    //4、循环收发数据 收数据需要创建新的线程,只接收服务端的数据
    //unsigned int ThreadId = 0;
    m_handle = (HANDLE)_beginthreadex(0, 0, &RecvThread, this, 0, 0);//倒数第二个参数0：创建即运行
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
    //关闭套接字
    if (m_sock != INVALID_SOCKET)closesocket(m_sock);
    //关闭线程 及线程句柄 and SOCKET
    m_isStop = true;
    if (m_handle)
    {
        if (WaitForSingleObject(m_handle, 100) == WAIT_TIMEOUT)
        {
            TerminateThread(m_handle, 0);
        }
        delete m_handle;
        m_handle = NULL;
        //CloseHandle(m_handle); 不知道为啥不通用这个
    }  
    
    //卸载库
    WSACleanup();
}

bool TcpClient::SendData(long ip, char* buf, int len)
{
    //判断合法性
    if (buf == NULL||len<=0)
    {
        cout << "TcpServer发送失败：数据包是空的\n" << endl;
        return false;
    }
    //if (ip == NULL)//客户端发送可以填空，因为初始化网络时 已经保存服务器的sock了 ，客户端也只和服务端交互
    //    ip = m_sock;
    //先发包大小

    if(send(m_sock, (char*)&len, sizeof(len), 0)<=0)
    {
        cout << __func__ << "发送失败" << endl;
        return false;
    }
    //再发包内容
    if (send(m_sock, buf, len, 0) <= 0)
    {
        cout << __func__ << "发送失败" << endl;
        return false;
    }
	return true;
}

void TcpClient::RecvData()
{
    //cout << "TcpClient::RecvData()" << endl;
    
    int recvLen = 0;
    int bufLen = 0;//用于接收包的大小
    sockaddr_in addr;
    int addrLen = sizeof(addr);
    char* buf=NULL;
    //接受数据也是阻塞的 多个客户端socket都要同时阻塞的去收，所以每一个客户端都要有一个线程去接收数据
    //先收包大小
    while (!m_isStop)
    {
        //cout << "ing1 " << endl;
        recvLen = recv(m_sock, (char*)&bufLen, sizeof(bufLen), 0);
        //cout << "ing2 " << endl;
        if (recvLen == 0)
        {
            cout << "recvData:和服务端连接已断开,关闭客户端" << endl;
            UnInitNet();
            return;
        }
        else if(recvLen>0)//接收到的包大小大于0
        {
            //cout << "recvLen>0" << endl;
            buf = new char[bufLen];
            int offset = 0;
            while (bufLen > 0)//只要数据还又没读取的（又没读取的recv就不会卡死），就接着读
            {
                recvLen = recv(m_sock, (char*)buf + offset, bufLen, 0);
                
                //接收的buf向后偏移 、buf剩余空间的大小
                offset += recvLen;
                bufLen -= recvLen;
            }
            //收完数据 传给中介者 todo处理完需要把buf堆区空间回收
            m_pMediator->DealData(m_sock, buf, offset);
        }
        //cout << "ing3" << endl;
    }
    //cout << "ing4 " << endl;
}
