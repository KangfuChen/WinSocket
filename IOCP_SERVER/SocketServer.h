#pragma once


#define DEFFAULT_BUFLEN     512

struct ClientSession
{
    WSAOVERLAPPED Overlaaped;
    SOCKET ClientSocket;
    char RecvBuf[DEFFAULT_BUFLEN];
    WSABUF WsaBuf;
};

class CSocketServer
{
private:
    SOCKET m_listenSocket;
    HANDLE m_IOCP;
    HANDLE m_accpetHeadle;
    short m_port;
    SYSTEM_INFO m_sysInfo;

    //-- 停止内部线程标识
    volatile bool m_finished; 

public:
    CSocketServer();
    virtual ~CSocketServer();

    //-- 开始服务
    int Start(short port);

    //-- 停止服务
    int Stop();

protected:
    void Init();

    static DWORD WINAPI WorkerThreadWrap(LPVOID pParam);
    static DWORD WINAPI AcceptThreadWrap(LPVOID pParam);

private:
    void WorkerProc();
    void AcceptProc();

};

