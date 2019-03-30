/**
 * risenshine_systemtasks_tcpserver
 * 
 * Task implementation for the TCP Server
 */

#include "risenshine_systemtasks.h"

#include "TCP_server.h"
#include "gui_user.h"

static void handleNewClient(int socket)
{
    gui_menuApi(&tcp_sendMessage, &tcp_recvMessage, socket);
}

void vTaskTCPServer(void *pvParameters)
{
    int socketfd;
    int newSocket = TCP_INVALIDSOCKET;

    tcp_createAndBindSocket(&socketfd);
    while(1) 
    {
        newSocket = tcp_acceptClients(socketfd);
        handleNewClient(newSocket);
        tcp_closeSocket(newSocket);
    }
}