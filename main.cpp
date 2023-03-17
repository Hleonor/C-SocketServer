#include <thread>
#include "MY_SOCKET.h"
using namespace std;



int main()
{
    MY_SOCKET my_socket;
    my_socket.startServer();
    /*char buffer[1024];
    thread send_thread(&MY_SOCKET::sendData, &my_socket, buffer); // 创建一个发送线程
    send_thread.detach();

    my_socket.readData();*/
    my_socket.watchConnecting();
    my_socket.closeSocket();
    return 0;
}
