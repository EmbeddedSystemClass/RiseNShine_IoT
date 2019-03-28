/*
 * TCP_server.c
 */

 #include "TCP_server.h"

static const char * TAG = "TCP_Module log:";

bool tcp_createAndBindSocket(int * const listen_sock)
{
    struct sockaddr_in destAddr;
    char addr_str[128];

    // Set up the address for the socket
    destAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    destAddr.sin_family = AF_INET;
    destAddr.sin_port = htons(PORT);
    inet_ntoa_r(destAddr.sin_addr, addr_str, sizeof(addr_str) - 1);

    // Create Socket
    *listen_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (*listen_sock < 0) {
        ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
        return false;
    }
    ESP_LOGI(TAG, "Socket created");

    // Bind the socket to the port
    int err = TCP_INVALIDSOCKET;
    err = bind(*listen_sock, (struct sockaddr *)&destAddr, sizeof(destAddr));
    if (err != 0) {
        ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
        return false;
    }
    ESP_LOGI(TAG, "Socket binded");
    
    return true;
}

int tcp_acceptClients(int listen_sock)
{
    int err = listen(listen_sock, 1);
    if (err != 0) {
        ESP_LOGE(TAG, "Error occured during listen: errno %d", errno);
        return TCP_INVALIDSOCKET;
    }
    ESP_LOGI(TAG, "Socket listening");

    struct sockaddr_in sourceAddr;

    uint32_t addrLen = sizeof(sourceAddr);
    int sock = accept(listen_sock, (struct sockaddr *)&sourceAddr, &addrLen);
    if (sock < 0) {
        ESP_LOGE(TAG, "Unable to accept connection: errno %d", errno);
        return TCP_INVALIDSOCKET;
    }
    ESP_LOGI(TAG, "Socket accepted");

    return sock;
}

        
bool tcp_sendMessage(int socket, char* payload, uint32_t len)
{
    int err = send(socket, payload, len, 0);
    if (err < 0) {
        ESP_LOGE(TAG, "Error occured during sending: errno %d", errno);
        return false;
    }
    return true;
}

int tcp_recvMessage(int socket, char* msgBuffer, uint32_t msgBufferSize)
{
    const uint32_t rxBufferSize = 128;
    char rxBuffer[rxBufferSize];
    char addr_str[128];
    struct sockaddr_in sourceAddr;

    int bytesRecv = recv(socket, rxBuffer, rxBufferSize - 1, 0);

    inet_ntoa_r(((struct sockaddr_in *)&sourceAddr)->sin_addr.s_addr, addr_str, sizeof(addr_str) - 1);
    ESP_LOGI(TAG, "Received %d bytes from %s:", bytesRecv, addr_str);
    ESP_LOGI(TAG, "%s", rxBuffer);

    if (bytesRecv < 0) {
        ESP_LOGE(TAG, "recv failed: errno %d", errno);
        return -1;
    }
    else if (bytesRecv == 0) {
        ESP_LOGI(TAG, "Connection closed");
        return 0;
    }
    memcpy(msgBuffer, rxBuffer, bytesRecv);
    msgBuffer[bytesRecv] = 0;

    return bytesRecv;
}

bool tcp_closeSocket(int socket)
{
    bool status = false;

    if (socket != TCP_INVALIDSOCKET) {
        ESP_LOGE(TAG, "Shutting down socket and restarting...");
        shutdown(socket, 0);
        close(socket);
        status = true;
    }
    return status;
}