// IOCP_SERVER.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "SocketServer.h"

/*
#define DEFAULT_PORT        "27015"
#define DEFFAULT_BUFLEN     512

#define NUM_WORKER_THREADS  4

struct ClientSession
{
    WSAOVERLAPPED Overlaaped;
    SOCKET ClientSocket;
    char RecvBuf[DEFFAULT_BUFLEN];
    WSABUF WsaBuf;
};

DWORD WINAPI AcceptThread(LPVOID p)
{
    SOCKET ListenSocket = (SOCKET)p;
    SOCKET ClientSocket = INVALID_SOCKET;
    int iSendResult;
    char recvbuf[DEFFAULT_BUFLEN];
    int recvbuflen = DEFFAULT_BUFLEN;
    ClientSession *Session;
    while (true)
    {
        DWORD Flag = 0;

        ClientSocket = accept(ListenSocket, (sockaddr*)NULL, NULL);
        if (ClientSocket == INVALID_SOCKET)
        {
            printf("accept failed: %d\n", WSAGetLastError());
            closesocket(ListenSocket);
            WSACleanup();
            return 1;
        }

        printf("++ Connection established ++\n");

        Session = (ClientSession *)malloc(sizeof(ClientSession));
        Session->ClientSocket = ClientSocket;
        Session->WsaBuf.buf = Session->RecvBuf;
        Session->WsaBuf.len = sizeof(Session->RecvBuf);

        CreateIoCompletionPort((HANDLE)ClientSocket, iocp, (ULONG_PTR)NULL, 0);

        ZeroMemory(&Session->Overlaaped, sizeof(WSAOVERLAPPED));
        WSARecv(ClientSocket, &Session->WsaBuf, 1, NULL, &Flag, (LPWSAOVERLAPPED)Session, NULL);
    }
}

DWORD WINAPI WorkerThread(LPVOID _iocp)
{
    DWORD id = GetCurrentThreadId();
    HANDLE iocp = (HANDLE)_iocp;

    ClientSession *Session = NULL;
    printf(":: WORKER STARTED (%d)::\n", id);
    while (true)
    {
        DWORD numberOfBytes;
        ULONG_PTR completionKey;
        LPOVERLAPPED overlapped;

        GetQueuedCompletionStatus(iocp, &numberOfBytes, &completionKey, &overlapped, INFINITE);

        Session = (ClientSession *)overlapped;

        if (numberOfBytes == 0)
        {
            printf("-- Connection closed (%d) --\n", id);
            shutdown(Session->ClientSocket, SD_SEND);
            closesocket(Session->ClientSocket);
            free(Session);
            //break;
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
    return 0;
}
*/

int main()
{
    CSocketServer server;
    server.Start(81);

    printf("input a char of 'q' to exit.\n" );
    while (true)
    {
        if (getchar() == 'q')
        {
            break;
        }
    }
    server.Stop();
    return 0;
}

