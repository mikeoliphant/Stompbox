#include <string>
#include "StompboxServer.h"

StompboxServer::StompboxServer()
{
}

void StompboxServer::Start()
{
#if _WIN32
    WSADATA wsaData;
    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        fprintf(stderr, "WSAStartup failed with error: %d\n", iResult);
        return;
    }
#endif
    
    runThread = new std::thread(&StompboxServer::RunServer, this);
}

void StompboxServer::Stop()
{
	runThread->join();
	delete runThread;

#if _WIN32
    WSACleanup();
#endif
}


void LogError(const char* err)
{
#if _WIN32
    fprintf(stderr, err, WSAGetLastError());
#else
    fprintf(stderr, err, errno);
#endif
}

void StompboxServer::RunServer()
{
    fprintf(stderr, "Open listen socket\n");

    StartListening();

    do
    {
        fprintf(stderr, "Start listening for connection\n");
        HandleConnection();
        fprintf(stderr, "Connection closed\n");
#if _WIN32
        Sleep(1000);
#else
        sleep(1);
#endif
        ClientSocket = INVALID_SOCKET;
    } while (true);
}

void StompboxServer::SetLineCallback(std::function<std::string(std::string)> callback)
{
    lineCallback = callback;
}


void StompboxServer::SendData(std::string const& data)
{
    if (ClientSocket != INVALID_SOCKET)
    {
        int iSendResult;

        iSendResult = send(ClientSocket, data.c_str(), (int)data.size(), 0);
        if (iSendResult == SOCKET_ERROR) {
            LogError("send failed with error: %d\n");
            closesocket(ClientSocket);
            return;
        }
    }
}

void StompboxServer::StartListening()
{
    ListenSocket = INVALID_SOCKET;

    struct addrinfo* result = NULL;
    struct addrinfo hints;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        fprintf(stderr, "getaddrinfo failed with error: %d\n", iResult);
        return;
    }

    // Create a SOCKET for connecting to server
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        LogError("socket failed with error: %ld\n");
        freeaddrinfo(result);
        return;
    }

    int enable = 1;
    if (setsockopt(ListenSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&enable, sizeof(enable)) < 0)
        LogError("setsockopt(SO_REUSEADDR) failed");

    // Setup the TCP listening socket
    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        LogError("bind failed with error: %d\n");
        freeaddrinfo(result);
        closesocket(ListenSocket);
        return;
    }

    freeaddrinfo(result);

    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        LogError("listen failed with error: %d\n");
        closesocket(ListenSocket);
        return;
    }
}

void StompboxServer::HandleConnection()
{
    ClientSocket = INVALID_SOCKET;

    // Accept a client socket
    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET) {
        LogError("accept failed with error: %d\n");
        closesocket(ListenSocket);
#if _WIN32
        WSACleanup();
#endif
        return;
    }

    fprintf(stderr, "Connection received\n");

    //// No longer need server socket
    //iResult = closesocket(ListenSocket);

    //if (iResult != 0)
    //    fprintf(stderr, "Close socket failed with %d\n", iResult);

    std::string dataStr;
    dataStr.clear();

    // Receive until the peer shuts down the connection
    do {
        iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0) {
            //fprintf(stderr, "Bytes received: %d\n", iResult);

            int startPos = 0;

            for (int i = 0; i < iResult; i++)
            {
                if (recvbuf[i] == '\n')
                    startPos = i + 1;

                if (recvbuf[i] == '\r')
                {
                    if (i != startPos)
                    {
                        dataStr.append(recvbuf + startPos, i - startPos);
                    }

                    if (lineCallback)
                    {
                        std::string err = lineCallback(dataStr);

                        if (err.empty())
                        {
                            SendData("Ok\r\n");
                        }
                        else
                        {
                            SendData("Error " + err + "\r\n");
                        }
                    }

                    //fprintf(stderr, "Got line: %s\n", dataStr.c_str());
                    dataStr.clear();

                    startPos = i + 1;
                }
            }

            if (startPos < iResult)
            {
                dataStr.append(recvbuf + startPos, iResult - startPos);
            }
        }
        else if (iResult == 0)
            fprintf(stderr, "Connection closing...\n");
        else {
            LogError("recv failed with error: %d\n");
            closesocket(ClientSocket);
            ClientSocket = INVALID_SOCKET;

            return;
        }

    } while (iResult > 0);

    // shutdown the connection since we're done
    iResult = shutdown(ClientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        LogError("shutdown failed with error: %d\n");
        closesocket(ClientSocket);
        ClientSocket = INVALID_SOCKET;

        return;
    }

    // cleanup
    closesocket(ClientSocket);
    ClientSocket = INVALID_SOCKET;

}
