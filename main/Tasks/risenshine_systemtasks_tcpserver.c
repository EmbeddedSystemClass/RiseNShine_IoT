/**
 * risenshine_systemtasks_tcpserver
 * 
 * Task implementation for the TCP Server
 */

#include "risenshine_systemtasks.h"

#include "TCP_server.h"

#include "esp_log.h"

static const char * TAG = "TCP task";

static int charToInt(char * chars, int len)
{
    int x = 0;
    int i;
    for(i = 0; i < len; i++) 
    {
        if(chars[i] >= '0' && chars[i] <= '9')
        {
            x = x*10 + (chars[i]-'0');
        }
    }
    return x;
}

void processTCPCommand(char* dataBuffer, int len)
{
    int numSteps = charToInt(dataBuffer, len);
    ESP_LOGI(TAG, "Received number: %d", numSteps);
    xQueueSend(qStepperMotorSteps, &numSteps, 0);
}

static void handleNewClient(int socket)
{
    char msgPayload[512];

    while (tcp_recvMessage(socket, msgPayload, sizeof(msgPayload)))
    {
        tcp_sendMessage(socket, msgPayload, sizeof(msgPayload));
    }
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