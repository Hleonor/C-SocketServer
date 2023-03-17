//
// Created by Hleonor on 2023-03-06.
//

#include <thread>
#include "MY_SOCKET.h"

int MY_SOCKET::creatASocket()
{
    // ��ʼ���׽��ֿ�
    WSADATA wsaData;
    int err = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (err != 0)
    {
        cout << "WSAStartup error" << endl;
        return -1;
    }
    // �����׽���
    // ���캯���ĵ�һ�������ĺ����ǣ�Э���壬�ڶ����������׽������ͣ�������������Э������
    serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serv_sock == INVALID_SOCKET)
    {
        cout << "�׽��ִ���ʧ��" << endl;
        return -1;
    }
    cout << "�׽��ִ����ɹ�" << endl;
    return 0;
}

int MY_SOCKET::bindSocket2IPAndPort()
{
    struct sockaddr_in serv_addr; // ��������ַ
    memset(&serv_addr, 0, sizeof(serv_addr)); // ÿ���ֽڶ���0���
    serv_addr.sin_family = AF_INET; // ʹ��IPv4��ַ
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // �����IP��ַ������ʹ�ñ���IP
    serv_addr.sin_port = htons(8888); // �˿ڣ�htons�����������ֽ���ת��Ϊ�����ֽ���
    int err = bind(serv_sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
    if (err == -1)
    {
        cout << "bind error" << endl;
        return -1;
    }
    cout << "�󶨳ɹ�" << endl;
    return 0;
}

int MY_SOCKET::listenSocket()
{
    // �������״̬���ȴ��û���������
    int err = listen(serv_sock, 20);
    if (err == -1)
    {
        cout << "listen error" << endl;
        return -1;
    }
    cout << "��ʼ����..." << endl;
    cout << endl;
    return 0;
}

int MY_SOCKET::acceptSocket()
{
    struct sockaddr_in client_addr;
    int client_addr_size = sizeof(client_addr); // �ͻ��˵�ַ�ṹ���С
    int the_client_sock = accept(serv_sock, (struct sockaddr *) &client_addr, &client_addr_size); // ���տͻ�������
    if (the_client_sock == -1)
    {
        cout << "client_socket failed" << endl;
        cout << endl;
        return -1;
    }
    cout << "�ͻ������ӳɹ�, �ͻ�����Ϣ: " << inet_ntoa(client_addr.sin_addr)
         << endl; // inet_ntoa�����������ֽ���IP��ַת��Ϊ�ַ���
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
    // ����һ��acceptSocket�����ļ����߳�
    thread acceptSocketThread(&MY_SOCKET::watchConnecting, this);
    acceptSocketThread.detach();
}

void MY_SOCKET::watchConnecting()
{
    try
    {
        while (true)
        {
            int the_client_sock = acceptSocket();
            if (the_client_sock == -1)
            {
                cout << "acceptSocket error" << endl;
                cout << endl;
                return;
            }
            client_sock_vector.push_back(the_client_sock);
            // ��ͻ��˷�����Ϣ�����ô��������̣߳�ֱ�ӷ��ͼ���
            // ���ͱ���ʱ��
            char *server_send;
            time_t now = time(0);
            server_send = ctime(&now);
            if (send(the_client_sock, server_send, strlen(server_send) + 1 + sizeof(char), 0) == SOCKET_ERROR)
            {
                cout << "����ʧ��" << endl;
                cout << endl;
            }
            else
            {
                cout << "���ͳɹ�" << endl;
                cout << endl;
            }
            // ���ͽ����󣬽������״̬���ȴ��ͻ��˷�����Ϣ
            thread readDataThread(&MY_SOCKET::readData, this, the_client_sock);
            readDataThread.detach();
        }
    }
    catch (exception e)
    {
        cout << e.what() << endl;
        cout << "watchConnecting error" << endl;
        cout << endl;
    }
}

void MY_SOCKET::readData(SOCKET client_sock)
{
    while (1)
    {
        // û���յ���Ϣ��ʱ��Ӧ������
        int err = recv(client_sock, server_recv, MAXBYTE, 0);
        if (err == SOCKET_ERROR)
        {
            cout << "����˽���ʧ��." << endl;
            cout << endl;
            return;
        }
        else
        {
            cout << "����˽��յ�����client " << client_sock << " ���͵���Ϣ����Ϣ�������ǣ�" << endl;
        }
        for (int i = 0; i < strlen(server_recv); i++)
        {
            cout << server_recv[i];
        }
        cout << endl;
        err = 0;
    }
}



