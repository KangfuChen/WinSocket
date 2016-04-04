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

    //-- ֹͣ�ڲ��̱߳�ʶ
    volatile bool m_finished; 

public:
    CSocketServer();
    virtual ~CSocketServer();

    //-- ��ʼ����
    int Start(short port);

    //-- ֹͣ����
    int Stop();

protected:
    void Init();

    static DWORD WINAPI WorkerThreadWrap(LPVOID pParam);
    static DWORD WINAPI AcceptThreadWrap(LPVOID pParam);

private:
    void WorkerProc();
    void AcceptProc();

};

