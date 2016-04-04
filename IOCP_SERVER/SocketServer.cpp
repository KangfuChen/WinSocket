#include "stdafx.h"
#include "SocketServer.h"


CSocketServer::CSocketServer()
{
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0)
    {
        printf("WSAStartup failed: %d\n", iResult);
    }

    m_listenSocket = INVALID_SOCKET;
}


CSocketServer::~CSocketServer()
{
    WSACleanup();
}

int CSocketServer::Start(short port)
{
    m_port = port;
    Init();

    return 0;
}

int CSocketServer::Stop()
{
    m_finished = true; 
    return 0;
}

void CSocketServer::Init()
{
    //-- 获取系统信息
    GetSystemInfo(&m_sysInfo);
    m_finished = false;

    addrinfo *result = NULL, *ptr = NULL, hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    char serverName[10];
    _itoa_s(m_port, serverName, 10);
    printf("serverName: %s\n", serverName);
    int iResult = getaddrinfo(NULL, serverName, &hints, &result);
    if (iResult != 0)
    {
        printf("getaddrinfo failed: %d\n", iResult);
        return;
    }

    m_listenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (m_listenSocket == INVALID_SOCKET)
    {
        printf("Error at socket(): %d\n", WSAGetLastError());
        freeaddrinfo(result);
        return ;
    }

    iResult = bind(m_listenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR)
    {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(m_listenSocket);
        return;
    }

    freeaddrinfo(result);

    m_IOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

    short proNum = (short)m_sysInfo.dwNumberOfProcessors + 2;
    for (short i = 0; i < proNum; i++)
    {
        DWORD threadID;
        if (CreateThread(NULL, 0, WorkerThreadWrap, this, 0, &threadID) == NULL)
        {
            printf("CreateThread failed.(%d)\n", GetLastError());
            break;
        }
        printf("CreateThread successfully,id %d\n", threadID);
    }

    iResult = listen(m_listenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR)
    {
        printf("Listen failed with error: %ld\n", WSAGetLastError());
        closesocket(m_listenSocket);
        return;
    }

    m_accpetHeadle = CreateThread(NULL, 0, AcceptThreadWrap, this, 0, NULL);
    if (m_accpetHeadle == NULL)
    {
        printf("CreateThread failed.(%d)\n", GetLastError());
    }
}

//--- 处理队列的状态
DWORD CSocketServer::WorkerThreadWrap(LPVOID pParam)
{
    CSocketServer *server = (CSocketServer *)pParam;
    server->WorkerProc();
    return 0;
}

//--- 监听客户端接入
DWORD CSocketServer::AcceptThreadWrap(LPVOID pParam)
{
    CSocketServer *server = (CSocketServer *)pParam;
    server->AcceptProc();
    return 0;
}

void CSocketServer::WorkerProc()
{
    DWORD id = GetCurrentThreadId();

    ClientSession *Session = NULL;
    printf(":: WORKER STARTED (%d)::\n", id);
    while (m_finished == false)
    {
        DWORD numberOfBytes;
        ULONG_PTR completionKey;
        LPOVERLAPPED overlapped;

        if (!GetQueuedCompletionStatus(m_IOCP, &numberOfBytes, &completionKey, &overlapped, INFINITE))
        {
            printf("GetQueuedCompletionStatus failed.(%d)\n", GetLastError());
            break;
        }

        Session = (ClientSession *)overlapped;

        if (numberOfBytes == 0)
        {
            printf("-- Connection closed (%d) --\n", id);
            shutdown(Session->ClientSocket, SD_SEND);
            closesocket(Session->ClientSocket);
            free(Session);
        }
        else
        {
            DWORD Flag = 0;
            printf(".. recived %d bytes (%d)\n", numberOfBytes, id);
            Session->RecvBuf[numberOfBytes] = 0;
            printf("%s\n", Session->RecvBuf);
            WSARecv(Session->ClientSocket, &Session->WsaBuf, 1, NULL, &Flag, overlapped, NULL);
        }
    }
}

//--- 等待客户端连接
void CSocketServer::AcceptProc()
{
    SOCKET ClientSocket = INVALID_SOCKET;
    int iSendResult;
    char recvbuf[DEFFAULT_BUFLEN];
    int recvbuflen = DEFFAULT_BUFLEN;
    ClientSession *Session = NULL;
    while (m_finished == false)
    {
        DWORD Flag = 0;

        ClientSocket = accept(m_listenSocket, (sockaddr*)NULL, NULL);
        if (ClientSocket == INVALID_SOCKET)
        {
            printf("accept failed: %d\n", WSAGetLastError());
            closesocket(m_listenSocket);
            return;
        }

        printf("++ Connection established ++\n");

        Session = (ClientSession *)malloc(sizeof(ClientSession));
        Session->ClientSocket = ClientSocket;
        Session->WsaBuf.buf = Session->RecvBuf;
        Session->WsaBuf.len = sizeof(Session->RecvBuf);

        CreateIoCompletionPort((HANDLE)ClientSocket, m_IOCP, (ULONG_PTR)NULL, 0);

        ZeroMemory(&Session->Overlaaped, sizeof(WSAOVERLAPPED));
        WSARecv(ClientSocket, &Session->WsaBuf, 1, NULL, &Flag, (LPWSAOVERLAPPED)Session, NULL);
    }
}