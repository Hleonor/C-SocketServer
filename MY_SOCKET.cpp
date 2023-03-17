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
    cout << "�ͻ������ӳɹ�, �ͻ�����Ϣ: " << inet_ntoa(client_addr.sin_addr) << " ==>> ��Ӧ�׽���Ϊ" << the_client_sock
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
        // ���ü�����д�ļ�������
        fd_set readfds;
        fd_set writefds;
        timeval timeout;
        timeout.tv_sec = 30;
        timeout.tv_usec = 0;
        FD_ZERO(&readfds);
        FD_ZERO(&writefds);
        FD_SET(serv_sock, &readfds); // ���׽��ּ��뵽��д�ļ��������У��˴��Ǽ����׽���
        FD_SET(serv_sock, &writefds);
        while (true)
        {
            /**
             * ��Ҫ˵���ģ�������������д���ļ����������˴������д���ļ�������Ҳ����select�����У�
             * ��ô��׼����һ���ͻ��˶�Ӧ���׽���(fd)�������ݵ�ʱ������select���Ϊ�ɶ��������ǿ�д�ģ����Ի����һֱ������
             * �Ӷ��޷���Ҳ�޷�д�����Դ˴�ֻ������ļ��������������ӿͻ��˶�Ӧ���׽��ֶ����ݵ�ʱ��ֱ��д��ʱ����Ϣ�����ͼ��ɡ�
             */
            int ret = select(0, &readfds, NULL, NULL, NULL);
            if (ret == -1)
            {
                cout << "select��������" << endl;
                cout << endl;
                return;
            }
            else if (ret == 0)
            {
                cout << "30s�ڣ�û��һ��fd׼���ö�д����" << endl;
                cout << endl;
                continue;
            }
            else
            {
                if (FD_ISSET(serv_sock, &readfds)) // ����������׽��־�����˵��һ���µ�����������
                {
                    int client_sock = acceptSocket();
                    if (client_sock == -1)
                    {
                        cout << "acceptSocket�������ӳ���" << endl;
                        cout << endl;
                        return;
                    }
                    // ���뵽�����ļ���������ȥ��
                    FD_SET(client_sock, &readfds);
                    FD_SET(client_sock, &writefds);

                    client_sock_vector.push_back(client_sock);
                }
                else
                {
                    // ����ǿͻ����׽��־�����˵���ͻ��������ݷ��͹���
                    for (int i = 0; i < client_sock_vector.size(); i++)
                    {
                        if (FD_ISSET(client_sock_vector[i], &readfds))
                        {
                            if (FD_ISSET(client_sock_vector[i], &writefds))
                            {
                                char *server_send;
                                ::time_t now = time(0);
                                string temp = "��ǰ������ϵͳʱ��Ϊ: " + string(ctime(&now));
                                server_send = temp.data();
                                if (send(client_sock_vector[i], server_send, strlen(server_send) + 1 + sizeof(char),
                                         0) ==
                                    SOCKET_ERROR)
                                {
                                    cout << "����ʧ��" << endl;
                                    cout << endl;
                                }
                                else
                                {
                                    cout << "���ͳɹ�" << endl;
                                    cout << endl;
                                }
                            }
                            // ���ͽ����󣬼����ͻ��˷��͵�����
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
                                cout << "�ͻ����Ѿ��ر�" << endl;
                                cout << endl;
                                closesocket(i);
                                FD_CLR(i, &readfds);
                            }
                            else
                            {
                                cout << "�ͻ��� " << client_sock_vector[i] << " ���͵�����: " << recv_buf << endl;
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





