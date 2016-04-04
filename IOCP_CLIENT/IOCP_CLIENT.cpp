// IOCP_CLIENT.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#define DEFAULT_PORT        "81"
#define DEFAULT_BUFLEN     512

int main(int argc, char *argv[])
{
    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo *result = NULL,
        *ptr = NULL,
        hints;
    char *sendbuf = "this is a test";
    //char recvbuf[DEFAULT_BUFLEN];
    int iResult;
    int recvbuflen = DEFAULT_BUFLEN;

    // Validate the parameters
    if (argc != 2) {
        printf("usage: %s server-name\n", argv[0]);
        return 1;
    }

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Attempt to connect to an address until one succeeds
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
            ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            return 1;
        }

        // Connect to server.
        iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }

    while (true)
    {
        char cmd[512];
        int ret = 0;
        printf("Input messages (input \"quit\" exit):\n>>");
        if ((ret = scanf_s("%s", cmd, 512)) <= 0)
        {
            printf("scanf_s %d\n", ret);
            break;
        }
        printf("cmd: %s\n", cmd);

        if (memcmp(cmd, "quit", 4) == 0)
        {
            break;
        }
        else
        {
            iResult = send(ConnectSocket, cmd, (int)strlen(cmd), 0);
            if (iResult == SOCKET_ERROR) {
                printf("send failed with error: %d\n", WSAGetLastError());
                closesocket(ConnectSocket);
                WSACleanup();
                return 1;
            }

            printf("Bytes Sent: %ld\n", iResult);
        }


    }




    //// Receive until the peer closes the connection
    //do {

    //    iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
    //    if (iResult > 0)
    //        printf("Bytes received: %d\n", iResult);
    //    else if (iResult == 0)
    //        printf("Connection closed\n");
    //    else
    //        printf("recv failed with error: %d\n", WSAGetLastError());

    //} while (iResult > 0);

    // shutdown the connection since no more data will be sent
    iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    // cleanup
    closesocket(ConnectSocket);
    WSACleanup();

    return 0;
}

