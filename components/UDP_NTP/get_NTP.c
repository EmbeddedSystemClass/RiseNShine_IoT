/* BSD Socket API Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>

typedef struct {
    uint8_t li_vn_mode;      // Eight bits. li, vn, and mode. [0]
                            // li.   Two bits.   Leap indicator. 
                            // vn.   Three bits. Version number of the protocol. 
                            // mode. Three bits. Client will pick mode 3 for client. 

    uint8_t stratum;         // Eight bits. Stratum level of the local clock. [1]
    uint8_t poll;            // Eight bits. Maximum interval between successive messages. [2]
    uint8_t precision;       // Eight bits. Precision of the local clock.[3]

    uint32_t rootDelay;      // 32 bits. Total round trip delay time. [4-7]
    uint32_t rootDispersion; // 32 bits. Max error aloud from primary clock source. [8-11]
    uint32_t refId;          // 32 bits. Reference clock identifier. [12-15]

    uint32_t refTm_s;        // 32 bits. Reference time-stamp seconds. [16-19]
    uint32_t refTm_f;        // 32 bits. Reference time-stamp fraction of a second. [20-23]

    uint32_t origTm_s;       // 32 bits. Originate time-stamp seconds. [24-27]
    uint32_t origTm_f;       // 32 bits. Originate time-stamp fraction of a second. [28-31]

    uint32_t rxTm_s;         // 32 bits. Received time-stamp seconds. [32-35]
    uint32_t rxTm_f;         // 32 bits. Received time-stamp fraction of a second. [36-39]

    uint32_t txTm_s;         // 32 bits and the most important field the client cares about. Transmit time-stamp seconds.
                             // [40-43]
    uint32_t txTm_f;         // 32 bits. Transmit time-stamp fraction of a second.  [44-47]
} ntp_packet_t;

/* The examples use simple WiFi configuration that you can set via
   'make menuconfig'.
   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define EXAMPLE_WIFI_SSID "mywifissid"
*/
#define EXAMPLE_WIFI_SSID CONFIG_WIFI_SSID
#define EXAMPLE_WIFI_PASS CONFIG_WIFI_PASSWORD

#ifdef CONFIG_EXAMPLE_IPV4
#define HOST_IP_ADDR CONFIG_EXAMPLE_IPV4_ADDR
#else
#define HOST_IP_ADDR CONFIG_EXAMPLE_IPV6_ADDR
#endif

#define PORT CONFIG_EXAMPLE_PORT

/* FreeRTOS event group to signal when we are connected & ready to make a request */
static EventGroupHandle_t wifi_event_group;

const int IPV4_GOTIP_BIT = BIT0;
#ifdef CONFIG_EXAMPLE_IPV6
const int IPV6_GOTIP_BIT = BIT1;
#endif

#define NTP_SERVER "time.nist.gov"
static const char *TAG = "example";

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch (event->event_id) {
    case SYSTEM_EVENT_STA_START:
        esp_wifi_connect();
        ESP_LOGI(TAG, "SYSTEM_EVENT_STA_START");
        break;
    case SYSTEM_EVENT_STA_CONNECTED:
#ifdef CONFIG_EXAMPLE_IPV6
        /* enable ipv6 */
        tcpip_adapter_create_ip6_linklocal(TCPIP_ADAPTER_IF_STA);
#endif
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        xEventGroupSetBits(wifi_event_group, IPV4_GOTIP_BIT);
        ESP_LOGI(TAG, "SYSTEM_EVENT_STA_GOT_IP");
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        /* This is a workaround as ESP32 WiFi libs don't currently auto-reassociate. */
        esp_wifi_connect();
        xEventGroupClearBits(wifi_event_group, IPV4_GOTIP_BIT);
#ifdef CONFIG_EXAMPLE_IPV6
        xEventGroupClearBits(wifi_event_group, IPV6_GOTIP_BIT);
#endif
        break;
    case SYSTEM_EVENT_AP_STA_GOT_IP6:
#ifdef CONFIG_EXAMPLE_IPV6
        xEventGroupSetBits(wifi_event_group, IPV6_GOTIP_BIT);
        ESP_LOGI(TAG, "SYSTEM_EVENT_STA_GOT_IP6");

        char *ip6 = ip6addr_ntoa(&event->event_info.got_ip6.ip6_info.ip);
        ESP_LOGI(TAG, "IPv6: %s", ip6);
#endif
    default:
        break;
    }
    return ESP_OK;
}

static void initialise_wifi(void)
{
    tcpip_adapter_init();
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = EXAMPLE_WIFI_SSID,
            .password = EXAMPLE_WIFI_PASS,
        },
    };
    ESP_LOGI(TAG, "Setting WiFi configuration SSID %s...", wifi_config.sta.ssid);
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK( esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
    ESP_ERROR_CHECK( esp_wifi_start() );
}

static void wait_for_ip()
{
#ifdef CONFIG_EXAMPLE_IPV6
    uint32_t bits = IPV4_GOTIP_BIT | IPV6_GOTIP_BIT;
#else
    uint32_t bits = IPV4_GOTIP_BIT;
#endif

    ESP_LOGI(TAG, "Waiting for AP connection...");
    xEventGroupWaitBits(wifi_event_group, bits, false, true, portMAX_DELAY);
    ESP_LOGI(TAG, "Connected to AP");
}

static void udp_client_task(void *pvParameters)
{
    char addr_str[128];

    while (1) {
        ESP_LOGI(TAG, "Start DNS lookup");

        struct addrinfo hints;
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_DGRAM;

        struct addrinfo *res;
        int err = getaddrinfo(NTP_SERVER, "123", &hints, &res);

        if(err != 0 || res == NULL) 
        {
            ESP_LOGE(TAG, "DNS lookup failed err=%d res=%p", err, res);
            freeaddrinfo(res);
            vTaskDelay(5000 / portTICK_PERIOD_MS);
            continue;
        }

        
        inet_ntoa_r(((struct sockaddr_in*)(res->ai_addr))->sin_addr.s_addr, addr_str, sizeof(addr_str) - 1);
        ESP_LOGI(TAG, "DNS lookup succeeded. IP=%s", addr_str);

        int sock = socket(res->ai_family, SOCK_DGRAM, res->ai_protocol);
        if (sock < 0) {
            ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
            freeaddrinfo(res);
            break;
        }
        ESP_LOGI(TAG, "Socket created");
        freeaddrinfo(res);

        while (1) 
        {
            ntp_packet_t payload;
            memset(&payload, 0, sizeof(payload));
            payload.li_vn_mode = 0xE3;

            int err = sendto(sock, &payload, sizeof(payload), 0, res->ai_addr, res->ai_addrlen);
            if (err < 0) {
                ESP_LOGE(TAG, "Error occured during sending: errno %d", errno);
                break;
            }
            ESP_LOGI(TAG, "Message sent");

            ntp_packet_t recv_buffer;
            memset(&recv_buffer, 0, sizeof(recv_buffer));
            struct sockaddr_in sourceAddr; // Large enough for both IPv4 or IPv6
            socklen_t socklen = sizeof(sourceAddr);
            int len = recvfrom(sock, &recv_buffer, sizeof(recv_buffer), 0, (struct sockaddr *)&sourceAddr, &socklen);
            inet_ntoa_r(sourceAddr.sin_addr.s_addr, addr_str, sizeof(addr_str) - 1);


            // Error occured during receiving
            if (len < 0) {
                ESP_LOGE(TAG, "recvfrom failed: errno %d", errno);
                break;
            }
            // Data received
            else {
                const uint32_t baselineTime = 2208988800UL;
                const uint32_t localTime = 28800UL;
                uint32_t recvTime = ntohl(recv_buffer.txTm_s);
                uint32_t convertedTime = recvTime - baselineTime - localTime;
                ESP_LOGI(TAG, "Received %d bytes from %s:", len, addr_str);
                ESP_LOGI(TAG, "Time: %d:%d:%d", convertedTime / 3600 % 60, convertedTime /60 % 60, convertedTime % 60);
            }

            vTaskDelay(5000 / portTICK_PERIOD_MS);
        }

        if (sock != -1) {
            ESP_LOGE(TAG, "Shutting down socket and restarting...");
            shutdown(sock, 0);
            close(sock);
        }
    }
    ESP_LOGI(TAG, "Task Deleted");
    vTaskDelete(NULL);
}

void app_main()
{
    ESP_ERROR_CHECK( nvs_flash_init() );
    initialise_wifi();
    wait_for_ip();

    xTaskCreate(udp_client_task, "udp_client", 4096, NULL, 5, NULL);
}
