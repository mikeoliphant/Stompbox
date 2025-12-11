#include <string>
#include "StompboxClient.h"

StompboxClient::StompboxClient()
{
}

void StompboxClient::Start()
{
    runThread = new std::thread(&StompboxClient::Connect, this);
}

void StompboxClient::Stop()
{
    runThread->join();
    delete runThread;
}

void StompboxClient::Connect()
{
    int iResult;

    struct addrinfo* result = NULL;
    struct addrinfo* ptr = NULL;
    struct addrinfo hints;

#if _WIN32
    WSADATA wsaData;
    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        fprintf(stderr, "WSAStartup failed with error: %d\n", iResult);
        return;
    }
#endif

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    iResult = getaddrinfo(DEFAULT_SERVER, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        fprintf(stderr, "getaddrinfo failed with error: %d\n", iResult);
#if _WIN32
        WSACleanup();
#endif
        return;
    }

    // Attempt to connect to an address until one succeeds
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
            ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
#if _WIN32
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
#endif
            return;
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
#if _WIN32
        WSACleanup();
#endif
        return;
    }

    // Receive until the peer closes the connection
    do {

        iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0)
            printf("Bytes received: %d\n", iResult);
        else if (iResult == 0)
            printf("Connection closed\n");
#if _WIN32
        else
            printf("recv failed with error: %d\n", WSAGetLastError());
#endif

    } while (iResult > 0);

    // cleanup
    closesocket(ConnectSocket);
#if _WIN32
    WSACleanup();
#endif
}

void StompboxClient::SetLineCallback(std::function<std::string(std::string)> callback)
{
    lineCallback = callback;
}


void StompboxClient::SendData(std::string const& data)
{
    if (ConnectSocket != INVALID_SOCKET)
    {
        int iSendResult;

        iSendResult = send(ConnectSocket, data.c_str(), (int)data.size(), 0);
        if (iSendResult == SOCKET_ERROR) {
#if _WIN32
            fprintf(stderr, "send failed with error: %d\n", WSAGetLastError());
#endif
            closesocket(ConnectSocket);
#if _WIN32
            WSACleanup();
#endif
            return;
        }
    }
}