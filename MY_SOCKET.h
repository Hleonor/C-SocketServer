//
// Created by Hleonor on 2023-03-06.
//

#ifndef SERVER_MY_SOCKET_H
#define SERVER_MY_SOCKET_H
#include <iostream>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

#include <vector>
#include <string.h>
using namespace std;

class MY_SOCKET
{
public:
    SOCKET serv_sock;
    // SOCKET client_sock;
    vector<SOCKET> client_sock_vector;
    vector<int> test_array;

    int creatASocket();
    int bindSocket2IPAndPort();
    int listenSocket();
    int acceptSocket();
    void readData(SOCKET a_client_sock);
    void sendData(char* buffer);
    void closeSocket();
    void startServer();
    void watchConnecting();

    char server_send[MAXBYTE];
    char server_recv[MAXBYTE];
    string server_send_change;
};

#endif //SERVER_MY_SOCKET_H
