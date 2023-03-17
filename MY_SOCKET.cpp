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
    int err = bind(serv_sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
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
    cout << endl;
    return 0;
}

int MY_SOCKET::acceptSocket()
{
    struct sockaddr_in client_addr;
    int client_addr_size = sizeof(client_addr); // 客户端地址结构体大小
    int the_client_sock = accept(serv_sock, (struct sockaddr *) &client_addr, &client_addr_size); // 接收客户端请求
    if (the_client_sock == -1)
    {
        cout << "client_socket failed" << endl;
        cout << endl;
        return -1;
    }
    cout << "客户端连接成功, 客户端信息: " << inet_ntoa(client_addr.sin_addr) << " ==>> 对应套接字为" << the_client_sock
         << endl; // inet_ntoa函数将网络字节序IP地址转换为字符串
    cout << endl;
    return the_client_sock;
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
    try
    {
        // 设置监听读写文件描述符
        fd_set readfds;
        fd_set writefds;
        timeval timeout;
        timeout.tv_sec = 30;
        timeout.tv_usec = 0;
        FD_ZERO(&readfds);
        FD_ZERO(&writefds);
        FD_SET(serv_sock, &readfds); // 将套接字加入到读写文件描述符中，此处是监听套接字
        FD_SET(serv_sock, &writefds);
        while (true)
        {
            /**
             * 需要说明的，第三个参数是写入文件描述符，此处如果将写入文件描述符也加入select函数中，
             * 那么当准备从一个客户端对应的套接字(fd)读入数据的时候，它被select检查为可读，而不是可写的，所以会造成一直阻塞，
             * 从而无法读也无法写，所以此处只加入读文件描述符，在往从客户端对应的套接字读数据的时候直接写入时间信息并发送即可。
             */
            int ret = select(0, &readfds, NULL, NULL, NULL);
            if (ret == -1)
            {
                cout << "select函数出错" << endl;
                cout << endl;
                return;
            }
            else if (ret == 0)
            {
                cout << "30s内，没有一个fd准备好读写操作" << endl;
                cout << endl;
                continue;
            }
            else
            {
                if (FD_ISSET(serv_sock, &readfds)) // 如果是倾听套接字就绪，说明一个新的连接请求建立
                {
                    int client_sock = acceptSocket();
                    if (client_sock == -1)
                    {
                        cout << "acceptSocket接收连接出错" << endl;
                        cout << endl;
                        return;
                    }
                    // 加入到监听文件描述符中去；
                    FD_SET(client_sock, &readfds);
                    FD_SET(client_sock, &writefds);

                    client_sock_vector.push_back(client_sock);
                }
                else
                {
                    // 如果是客户端套接字就绪，说明客户端有数据发送过来
                    for (int i = 0; i < client_sock_vector.size(); i++)
                    {
                        if (FD_ISSET(client_sock_vector[i], &readfds))
                        {
                            if (FD_ISSET(client_sock_vector[i], &writefds))
                            {
                                char *server_send;
                                ::time_t now = time(0);
                                string temp = "当前服务器系统时间为: " + string(ctime(&now));
                                server_send = temp.data();
                                if (send(client_sock_vector[i], server_send, strlen(server_send) + 1 + sizeof(char),
                                         0) ==
                                    SOCKET_ERROR)
                                {
                                    cout << "发送失败" << endl;
                                    cout << endl;
                                }
                                else
                                {
                                    cout << "发送成功" << endl;
                                    cout << endl;
                                }
                            }
                            // 发送结束后，监听客户端发送的数据
                            char recv_buf[1024];
                            int recv_len = recv(client_sock_vector[i], recv_buf, sizeof(recv_buf), 0);
                            if (recv_len == -1)
                            {
                                cout << "recv error" << endl;
                                cout << endl;
                                return;
                            }
                            else if (recv_len == 0)
                            {
                                cout << "客户端已经关闭" << endl;
                                cout << endl;
                                closesocket(i);
                                FD_CLR(i, &readfds);
                            }
                            else
                            {
                                cout << "客户端 " << client_sock_vector[i] << " 发送的数据: " << recv_buf << endl;
                                cout << endl;
                            }
                        }
                    }
                }
            }
        }
    }
    catch (exception e)
    {
        cout << e.what() << endl;
        cout << "watchConnecting error" << endl;
        cout << endl;
    }
}





