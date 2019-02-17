/*
 * TCP_server.c
 */

 #include "TCP_server.h"

static const char *TAG = "TCP_Module log:";

static int charToInt(char * chars, int len)
{
    int x;
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

bool tcp_createAndBindSocket(struct sockaddr_in *destAddr, int * listen_sock)
{
    // Set up the address for the socket
    destAddr->sin_addr.s_addr = htonl(INADDR_ANY);
    destAddr->sin_family = AF_INET;
    destAddr->sin_port = htons(PORT);
    //inet_ntoa_r(destAddr.sin_addr, addr_str, sizeof(addr_str) - 1);

    // Create Socket
    *listen_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (*listen_sock < 0) {
        ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
        return false;
    }
    ESP_LOGI(TAG, "Socket created");

    // Bind the socket to the port
    int err = -1;
    err = bind(*listen_sock, (struct sockaddr *)&destAddr, sizeof(destAddr));
    if (err != 0) {
        ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
        return false;
    }
    ESP_LOGI(TAG, "Socket binded");
    
    return true;
}

void tcp_acceptClients(int listen_sock, void (*callback_ptr)(char* buffer, int len) )
{
    char rx_buffer[128];
    char addr_str[128];

    while (1) 
    {
        int err = listen(listen_sock, 1);
        if (err != 0) {
            ESP_LOGE(TAG, "Error occured during listen: errno %d", errno);
            break;
        }
        ESP_LOGI(TAG, "Socket listening");

        struct sockaddr_in sourceAddr;

        uint addrLen = sizeof(sourceAddr);
        int sock = accept(listen_sock, (struct sockaddr *)&sourceAddr, &addrLen);
        if (sock < 0) {
            ESP_LOGE(TAG, "Unable to accept connection: errno %d", errno);
            break;
        }
        ESP_LOGI(TAG, "Socket accepted");

        while (1) {
            int len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
            // Error occured during receiving
            if (len < 0) {
                ESP_LOGE(TAG, "recv failed: errno %d", errno);
                break;
            }
            // Connection closed
            else if (len == 0) {
                ESP_LOGI(TAG, "Connection closed");
                break;
            }
            // Data received
            else {
                inet_ntoa_r(((struct sockaddr_in *)&sourceAddr)->sin_addr.s_addr, addr_str, sizeof(addr_str) - 1);
                rx_buffer[len] = 0; // Null-terminate whatever we received and treat like a string
                ESP_LOGI(TAG, "Received %d bytes from %s:", len, addr_str);
                ESP_LOGI(TAG, "%s", rx_buffer);

                (*callback_ptr)(rx_buffer, len);
                int err = send(sock, rx_buffer, len, 0);
                if (err < 0) {
                    ESP_LOGE(TAG, "Error occured during sending: errno %d", errno);
                    break;
                }
            }
        }

        if (sock != -1) {
            ESP_LOGE(TAG, "Shutting down socket and restarting...");
            shutdown(sock, 0);
            close(sock);
        }
    }
}