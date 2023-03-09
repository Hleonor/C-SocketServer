//
// Created by Hleonor on 2023-03-06.
//

#include <thread>
#include "MY_SOCKET.h"

int MY_SOCKET::creatASocket()
{
    // 初始化套接字库
    WSADATA wsaData;
    int err = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (err != 0)
    {
        cout << "WSAStartup error" << endl;
        return -1;
    }
    // 创建套接字
    // 构造函数的第一个参数的含义是：协议族，第二个参数是套接字类型，第三个参数是协议类型
    serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serv_sock == INVALID_SOCKET)
    {
        cout << "套接字创建失败" << endl;
        return -1;
    }
    cout << "套接字创建成功" << endl;
    return 0;
}

int MY_SOCKET::bindSocket2IPAndPort()
{
    struct sockaddr_in serv_addr; // 服务器地址
    memset(&serv_addr, 0, sizeof(serv_addr)); // 每个字节都用0填充
    serv_addr.sin_family = AF_INET; // 使用IPv4地址
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // 具体的IP地址，这里使用本机IP
    serv_addr.sin_port = htons(8888); // 端口，htons函数将主机字节序转换为网络字节序
    int err = bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if (err == -1)
    {
        cout << "bind error" << endl;
        return -1;
    }
    cout << "绑定成功" << endl;
    return 0;
}

int MY_SOCKET::listenSocket()
{
    // 进入监听状态，等待用户发起请求
    int err = listen(serv_sock, 20);
    if (err == -1)
    {
        cout << "listen error" << endl;
        return -1;
    }
    cout << "开始监听..." << endl;
    return 0;
}

int MY_SOCKET::acceptSocket()
{
    struct sockaddr_in client_addr;
    int client_addr_size = sizeof(client_addr); // 客户端地址结构体大小
    int the_client_sock = accept(serv_sock, (struct sockaddr*)&client_addr, &client_addr_size); // 接收客户端请求
    if (the_client_sock == -1)
    {
        cout << "client_socket failed" << endl;
        return -1;
    }
    client_sock.push_back(the_client_sock);
    cout << "client_socket success, client info: "<< inet_ntoa(client_addr.sin_addr) << endl; // inet_ntoa函数将网络字节序IP地址转换为字符串
    return 0;
}

void MY_SOCKET::readData()
{
    while (1)
    {
        // 没接收到信息的时候应该阻塞
        int err = recv(client_sock, server_recv, MAXBYTE, 0);
        if (err == SOCKET_ERROR)
        {
            cout << "服务端接收失败." << endl;
            return ;
        }
        else
        {
            cout << "服务端接收成功." << endl;
        }
        for (int i = 0; i < strlen(server_recv); i++)
        {
            cout << server_recv[i];
        }
        cout << endl;
        err = 0;
    }
}

void MY_SOCKET::sendData(char* buffer)
{
    while (1)
    {
        cout << "请输入要发送的信息:";
        server_send_change.clear();
        cin >> server_send_change;
        for (int i = 0; i < server_send_change.size(); i++)
        {
            server_send[i] = server_send_change[i];
        }
        if (send(serv_sock, server_send, server_send_change.length() + 1 + sizeof(char), 0) == SOCKET_ERROR)
        {
            cout << "发送失败" << endl;
        }
        else
        {
            cout << "发送成功" << endl;
        }
    }
}



void MY_SOCKET::closeSocket()
{
    WSACleanup();
    // closesocket(client_sock);
    closesocket(serv_sock);
}

void MY_SOCKET::startServer()
{
    creatASocket();
    bindSocket2IPAndPort();
    listenSocket();
    // 创建一个acceptSocket函数的监听线程
    thread acceptSocketThread(&MY_SOCKET::watchConnecting, this);
    acceptSocketThread.detach();
}

void MY_SOCKET::watchConnecting()
{

}


