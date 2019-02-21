/**
 * http_get.c
 */

#include "http_get.h"

static const char* TAG = "HTTP Get";

static const char *REQUEST = "GET " WEB_URL " HTTP/1.0\r\n"
    "Host: "WEB_SERVER"\r\n"
    "User-Agent: esp-idf/1.0 esp32\r\n"
    "\r\n";

int http_connectSocket()
{
    const struct addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM,
    };
    struct addrinfo *res;
    int s = -1;
    char addr_str[128];


    int err = getaddrinfo(WEB_SERVER, "80", &hints, &res);

    if(err != 0 || res == NULL) {
        ESP_LOGE(TAG, "DNS lookup failed err=%d res=%p", err, res);
        return -1;
    }

    /* Code to print the resolved IP.*/
    inet_ntoa_r(((struct sockaddr_in *)&res)->sin_addr.s_addr, addr_str, sizeof(addr_str) - 1);
    ESP_LOGI(TAG, "DNS lookup succeeded. IP=%s", addr_str);

    s = socket(res->ai_family, res->ai_socktype, 0);
    if(s < 0) {
        ESP_LOGE(TAG, "Failed to allocate socket.");
        freeaddrinfo(res);
        return -1;
    }
    ESP_LOGI(TAG, "allocated socket");

    if(connect(s, res->ai_addr, res->ai_addrlen) != 0) {
        ESP_LOGE(TAG, "socket connect failed errno=%d", errno);
        close(s);
        freeaddrinfo(res);
        return -1;
    }

    ESP_LOGI(TAG, "connected");
    freeaddrinfo(res);

    return s;
}

void http_sendRequest(int s, char payload[], int payloadSize)
{
    int nRead;
    char recv_buffer[128];
    int cursor = 0;


    if (write(s, REQUEST, strlen(REQUEST)) < 0) {
        ESP_LOGE(TAG, "socket send failed");
        close(s);
        return;
    }
    ESP_LOGI(TAG, "socket send success");

    struct timeval receiving_timeout;
    receiving_timeout.tv_sec = 5;
    receiving_timeout.tv_usec = 0;
    if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &receiving_timeout,
            sizeof(receiving_timeout)) < 0) {
        ESP_LOGE(TAG, "failed to set socket receiving timeout");
        close(s);
        return;
    }
    ESP_LOGI(TAG, "set socket receiving timeout success");

    /* Read HTTP response */
    // clear buffer first
    memset(payload, 0, payloadSize);
    do {
        memset(recv_buffer, 0, sizeof(recv_buffer));
        nRead = read(s, payload, payloadSize-1);
        for(int i = 0; i < nRead; i++)
        {

        }
    } while (nRead > 0);
    
    

    if (nRead > 0) {
        ESP_LOGE(TAG, "Error! Buffer not large enough to store payload.");
        close(s);
        return;
    }

    ESP_LOGI(TAG, "done reading from socket. Last read return=%d errno=%d", nRead, errno);
    close(s);
}
