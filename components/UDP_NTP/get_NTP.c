/*
 * get_NTP.c
 * 
 */

#include "get_NTP.h"

#define NTP_SERVER "time.nist.gov"
static const char *TAG = "get_ntp";

int udp_getUDPsocket()
{
    struct addrinfo hints;
    struct addrinfo *res;
    char addr_str[128];

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    
    int err = getaddrinfo(NTP_SERVER, "123", &hints, &res);

    if(err != 0 || res == NULL) 
    {
        ESP_LOGE(TAG, "DNS lookup failed err=%d res=%p", err, res);
        freeaddrinfo(res);
        return -1;
    }

    
    inet_ntoa_r(((struct sockaddr_in*)(res->ai_addr))->sin_addr.s_addr, addr_str, sizeof(addr_str) - 1);
    ESP_LOGI(TAG, "DNS lookup succeeded. IP=%s", addr_str);

    int sock = socket(res->ai_family, SOCK_DGRAM, res->ai_protocol);
    if (sock < 0) {
        ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
        freeaddrinfo(res);
        return -1;
    }
    ESP_LOGI(TAG, "Socket created");

    //set recv socket as non-blocking
    fcntl(sock, F_SETFL, O_NONBLOCK);

    err = connect(sock, res->ai_addr, res->ai_addrlen);
    if (err == -1)
    {
        ESP_LOGE(TAG, "Error connecting to server");
        freeaddrinfo(res);
        close(sock);
        return -1;
    }

    freeaddrinfo(res);

    return sock;
}

void udp_sendMsg(int sock)
{
    ntp_packet_t payload;
    memset(&payload, 0, sizeof(payload));
    payload.li_vn_mode = 0xE3;

    int err = send(sock, &payload, sizeof(payload), 0);
    if (err < 0) {
        ESP_LOGE(TAG, "Error occured during sending: errno %d", errno);
        return;
    }
    ESP_LOGI(TAG, "Message sent");
}

void udp_recvMsg(int sock, void(*callback_ptr)(uint32_t *data))
{
    ntp_packet_t recv_buffer;
    memset(&recv_buffer, 0, sizeof(recv_buffer));
    int len = recv(sock, &recv_buffer, sizeof(recv_buffer), 0);

    // Error occured during receiving
    if (len < 0) {
        ESP_LOGE(TAG, "recvfrom failed: errno %d", errno);
        return;
    }
    // Data received
    else {
        uint32_t recvTime = ntohl(recv_buffer.txTm_s);
        (*callback_ptr)(&recvTime);
        ESP_LOGI(TAG, "Received %d bytes", len);
    }

    if (sock != -1) {
        ESP_LOGI(TAG, "Shutting down socket and restarting...");
        shutdown(sock, 0);
        close(sock);
    }
}
