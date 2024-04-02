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
//打开网络：加载库 创建套接字 绑定ip 监听
bool TcpServer::InitNet()
{
    cout <<"tcpSever:"<< __func__ << endl;
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
    return true;
}
//接受链接线程函数
unsigned __stdcall TcpServer::AcceptThread(void* lpVoid)
{
    TcpServer* pThis = (TcpServer*)lpVoid;//用服务端socket  静态函数使用 成员属性要有对象
    sockaddr_in addrClient;
    int addrLen = sizeof(addrClient);
    unsigned int threadId = 0;
    while (!pThis->m_isStop)
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
    //关闭套接字,非空且是合法的
    if (m_sock&&m_sock!=INVALID_SOCKET)closesocket(m_sock);
    for (map<unsigned int, SOCKET>::iterator ite = m_threadIdToSocket.begin(); ite != m_threadIdToSocket.end(); )
    {
        if(ite->second&&ite->second!=INVALID_SOCKET)
            closesocket(ite->second);
        //socket关闭了，是无效的了，所以从map移除
        ite = m_threadIdToSocket.erase(ite);
    }
    //这样接收和连接线程干完把本轮任务就结束了
    m_isStop = true;
    //关闭线程 及线程句柄 
    for (list<HANDLE>::iterator ite=m_handleList.begin();ite!=m_handleList.end();)
    {
        //等一会，让线程函数执行到while(!m_isStop)结束死循环 自动退出
        if ((*ite))
        {
            //等100毫秒还没结束就杀死
            if(WaitForSingleObject((*ite),100)==WAIT_TIMEOUT)
                TerminateThread((*ite), 0);
            delete (*ite);
            (*ite) = NULL;
        }
        //回收list无效节点
        ite = m_handleList.erase(ite);
    }
    
    //卸载库
    WSACleanup();
}
//发数据:long类型 可以兼容ip地址和socket
bool TcpServer::SendData(long ip, char* buf, int len)
{
    //判断合法性
    if (buf == NULL ||len<=0)
    {
        cout <<__func__<< " TcpServer发送失败：数据包是空的 或者 len<=0\n" << endl;
        return false;
    }
    //先发包大小
    if (send(ip, (char*)&len, sizeof(len), 0) <= 0)
    {
        cout << __func__ << "send error:" <<WSAGetLastError()<< endl;
        return false;
    }
    //cout << "Server send success!" << endl;
    //再发包内容
    if(send(ip, buf, len, 0)<=0)
    {
        cout << __func__ << "send error:" << WSAGetLastError() << endl;
        return false;
    }
    //cout << "Server send success!" << endl;
	return true;
}
//收数据
void TcpServer::RecvData()
{
    //连接成功后 创建线程（这个线程创建既运行）可能id 和socket还没放到map中呢 导致娶不到socket
    //1、休眠一会
    Sleep(100);
    //2、获取当前线程的 socket
    SOCKET sock = m_threadIdToSocket[GetCurrentThreadId()];
    if (!sock || sock == INVALID_SOCKET)
    {
        cout << __func__ << "socket error" << endl;
        return;
    }
    int recvLen = 0;
    int bufLen = 0;//用于接收包的大小
    sockaddr_in addr;
    int addrLen = sizeof(addr);
    char* buf = NULL;
    //接受数据也是阻塞的 多个客户端socket都要同时阻塞的去收，所以每一个客户端都要有一个线程去接收数据
    //先收包大小
    while (!m_isStop)
    {
        recvLen = recv(sock, (char*)&bufLen, sizeof(bufLen), 0);
        if (recvLen == 0)
        {
            //接受数据失败
            cout << __func__ << "recv error:" << WSAGetLastError() << endl;
            return;
            //返回值为0 表示连接已断开
            //关闭线程 句柄 socket
            //closesocket(sock);
            /*for (HANDLE handle : m_handleList)
            {
                if()
            }
            CloseHandle()*/
            //return;
        }
        else//接收到的包大小大于0
        {
            //知道应该收包的大小，所以就new这么大的空间
            /////mtu1460，客户端一次发的可能包比较大比如2000 所以会分包，所以接收就需要循环接受（这样才能接收完整）
            buf = new char[bufLen];
            memset(buf, 0, bufLen);
            int offset = 0;//存储累计接收到的数据

            //if(recv(sock, (char*)&buf, bufLen, 0)>0)
            while (bufLen > 0)
            {
                recvLen = recv(sock, (char*)buf + offset, bufLen, 0);
                if (recvLen > 0)
                {
                    //接收的buf向后偏移 、buf剩余空间的大小
                    offset += recvLen;//累计接收数据量
                    bufLen -= recvLen;//还需要接收的数据量
                }
                else
                {
                    //接受数据失败
                    cout << __func__ << "recv error:" << WSAGetLastError() << endl;
                    return;
                }
            }
            //收完数据 传给中介者 todo处理完需要把buf堆区空间回收
            m_pMediator->DealData(sock, buf, offset);
        }
    }
    //再根据包大小 接受真正的数据包
    //while (1)
    //{
    //    //recv(sock,)
    //}

}
//接收数据线程函数 
unsigned __stdcall TcpServer::RecvThread(void* lpVoid)
{
    TcpServer* pThis = (TcpServer*)lpVoid;
    pThis->RecvData();
    return 0;
}


