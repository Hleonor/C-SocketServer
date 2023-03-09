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
    int err = bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
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
    return 0;
}

int MY_SOCKET::acceptSocket()
{
    struct sockaddr_in client_addr;
    int client_addr_size = sizeof(client_addr); // �ͻ��˵�ַ�ṹ���С
    int the_client_sock = accept(serv_sock, (struct sockaddr*)&client_addr, &client_addr_size); // ���տͻ�������
    if (the_client_sock == -1)
    {
        cout << "client_socket failed" << endl;
        return -1;
    }
    client_sock.push_back(the_client_sock);
    cout << "client_socket success, client info: "<< inet_ntoa(client_addr.sin_addr) << endl; // inet_ntoa�����������ֽ���IP��ַת��Ϊ�ַ���
    return 0;
}

void MY_SOCKET::readData()
{
    while (1)
    {
        // û���յ���Ϣ��ʱ��Ӧ������
        int err = recv(client_sock, server_recv, MAXBYTE, 0);
        if (err == SOCKET_ERROR)
        {
            cout << "����˽���ʧ��." << endl;
            return ;
        }
        else
        {
            cout << "����˽��ճɹ�." << endl;
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
        cout << "������Ҫ���͵���Ϣ:";
        server_send_change.clear();
        cin >> server_send_change;
        for (int i = 0; i < server_send_change.size(); i++)
        {
            server_send[i] = server_send_change[i];
        }
        if (send(serv_sock, server_send, server_send_change.length() + 1 + sizeof(char), 0) == SOCKET_ERROR)
        {
            cout << "����ʧ��" << endl;
        }
        else
        {
            cout << "���ͳɹ�" << endl;
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
    // ����һ��acceptSocket�����ļ����߳�
    thread acceptSocketThread(&MY_SOCKET::watchConnecting, this);
    acceptSocketThread.detach();
}

void MY_SOCKET::watchConnecting()
{

}


