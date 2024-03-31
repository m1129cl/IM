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
//打开网络：加载库 创建套接字 绑定ip 监听
bool TcpServer::InitNet()
{
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
    //3.绑定ip地址
    sockaddr_in addrServer;
    addrServer.sin_family = AF_INET;
    addrServer.sin_port = htons(_DEF_TCP_PORT);//把主机字节序（小端）转换成网络字节序（大端）
    addrServer.sin_addr.S_un.S_addr = INADDR_ANY;//绑定任意网卡
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

    //4.监听
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
    //接受连接（可以连接多个客户端的连接 阻塞的一直接受链接 仅执行一个任务要用多线程）
    //使用 _beginthreadex—>endthreadex先回收空间 在去调用existthread ;可以自动回收C++运行时库的资源 createthread需要我们手动帮他回收
    HANDLE handle =(HANDLE) _beginthreadex(0, 0, &AcceptThread, this, 0, NULL);//标志位0：创建及运行；thradId
    if (handle)
        m_handleList.push_back(handle);
    return false;
}
//接受链接线程函数
unsigned __stdcall TcpServer::AcceptThread(void* lpVoid)
{
    TcpServer* pThis = (TcpServer*)lpVoid;//用服务端socket  静态函数使用 成员属性要有对象
    sockaddr_in addrClient;
    int addrLen = sizeof(addrClient);
    unsigned int threadId = 0;
    while (1)
    {
        //接受链接
        ///静态成员函数不能访问 非静态成员，静态函数内部要有对象或指针去调用
        SOCKET sock = accept(pThis->m_sock, (sockaddr*)&addrClient,&addrLen);
        cout << "accpet success,ip:" << inet_ntoa(addrClient.sin_addr) << endl;
        //给连接成功的客户端 要创建一个接收数据的线程
        //线程句柄 用于结束线程 回收回收内核对象时使用
        HANDLE handle = (HANDLE)_beginthreadex(0, 0, &RecvThread, pThis, 0, &threadId);//标志位0：创建及运行；thradId
        if(handle)
            pThis->m_handleList.push_back(handle);
        //把线程id 、socket保存到map中
        pThis->m_threadIdToSocket[threadId] = sock;
    }
    return 0;
}

//关闭网络
void TcpServer::UnInitNet()
{
}
//发数据
bool TcpServer::SendData(long ip, char* buf, int len)
{
    //判断合法性
    //宪法包大小
    //再发包内容
    send(ip, buf, len, 0);
	return false;
}
//收数据
void TcpServer::RecvData()
{
    SOCKET sock = m_threadIdToSocket[GetCurrentThreadId()];
    //接受数据也是阻塞的 多个客户端socket都要同时阻塞的去收，所以每一个客户端都要有一个线程去接收数据
    while (1)
    {
        //recv(sock,)
    }

}
//接收数据线程函数 
unsigned __stdcall TcpServer::RecvThread(void* lpVoid)
{
    TcpServer* pThis = (TcpServer*)lpVoid;
    pThis->RecvData();
    return 0;
}


