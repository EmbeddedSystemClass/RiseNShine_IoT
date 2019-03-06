/**
 * risenshine_systemtasks_background
 * 
 * Task implementation for low-priority tasks
 */

#include "risenshine_systemtasks.h"

#include "http_get.h"
#include "get_NTP.h"

#include "esp_log.h"

static const char * TAG = "Background task";

#define PAYLOADSIZE 1024
#define BASELINETIME 2208988800UL
#define LOCALTIME 28800UL

static bool parseSunriseSunsetJSON(const char * const data, strSunriseSunsetTimes_t * dataHolder)
{
    const cJSON *sunriseTime = NULL;
    const cJSON *sunsetTime = NULL;
    const cJSON *results = NULL;

    cJSON * apiResult = cJSON_Parse(data);
    if (apiResult == NULL)
    {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
            ESP_LOGE(TAG, "Error before: %s", error_ptr);
        }
        cJSON_Delete(apiResult);
        return false;
    }

    results = cJSON_GetObjectItem(apiResult, "results");

    sunriseTime = cJSON_GetObjectItem(results, "sunrise");
    if(!cJSON_IsString(sunriseTime))
    {
        cJSON_Delete(apiResult);
        return false;
    }

    sunsetTime = cJSON_GetObjectItem(results, "sunset");
    if(!cJSON_IsString(sunsetTime))
    {
        cJSON_Delete(apiResult);
        return false;
    }

    unsigned int throwaway;
    sscanf(sunriseTime->valuestring, "%u-%u-%uT%u:%u:%u+%u:%u", \
        &throwaway, &throwaway, &throwaway, &(dataHolder->sunriseTime.hour), \
        &(dataHolder->sunriseTime.minute), &(dataHolder->sunriseTime.second), \
        &throwaway, &throwaway);

    sscanf(sunsetTime->valuestring, "%u-%u-%uT%u:%u:%u+%u:%u", \
        &throwaway, &throwaway, &throwaway, &(dataHolder->sunsetTime.hour), \
        &(dataHolder->sunsetTime.minute), &(dataHolder->sunsetTime.second), \
        &throwaway, &throwaway);

    cJSON_Delete(apiResult);
    return true;
}

static char * getHTMLContent(char * data)
{
    return strstr(data, "\r\n\r\n");
}

static timePackage_t createPackage(timeType_e type, timeFormat_t *time)
{
    timePackage_t package = {
        .timeType = type,
        .timeData = *time
    };
    return package;
}

static void sendTimeDataToQueue(timeType_e type, timeFormat_t * data)
{
    timePackage_t package = createPackage(sunriseTime, data->sunriseTime);
    xQueueSend(qClockUpdate, &package, 0);

    package = createPackage(sunsetTime, data->sunsetTime);
    xQueueSend(qClockUpdate, &package, 0);
}

void processNTPResult(uint32_t *data)
{
    uint32_t convertedTime = *data - BASELINETIME - LOCALTIME;
    ESP_LOGI(TAG, "Received UNIX time: %d", convertedTime);
    timePackage_t package = {
        .timeType = currentTime,
        .timeData.hour = convertedTime / 3600 % 24,
        .timeData.minute = convertedTime / 60 % 60,
        .timeData.second = convertedTime % 60
    };
    xQueueSend(qClockUpdate, &package, 0);
}

static void convertToLocalTime(timeFormat_t *data)
{
    const int hourDiff = 8;
    if (data->hour < 8)
    {
        data->hour = data->hour + 24 - hourDiff;
    }
    else
    {
        data->hour -= 8;
    }
}

void vTaskIdleComputations(void *pvParameters)
{
    char payload[PAYLOADSIZE];
    char * htmlContent = NULL;
    int httpSocket = -1;
    int udpSocket = -1;
    strSunriseSunsetTimes_t dataHolder;
    bool parseSuccess = false;
 
    while(1)
    {
        httpSocket = http_connectSocket();
        http_sendRequest(httpSocket, payload, PAYLOADSIZE);

        //remove header from content
        htmlContent = getHTMLContent(payload);
        if (htmlContent == NULL) 
        {
            ESP_LOGE(TAG, "Cannot separate HTML header");
            continue;
        }

        parseSuccess = parseSunriseSunsetJSON(htmlContent, &dataHolder);
        if (parseSuccess)
        {
            convertToLocalTime(&(dataHolder.sunriseTime));
            convertToLocalTime(&(dataHolder.sunsetTime));
            ESP_LOGI(TAG, "Sunrise time: %u:%u:%u", dataHolder.sunriseTime.hour, dataHolder.sunriseTime.minute, dataHolder.sunriseTime.second);
            ESP_LOGI(TAG, "Sunset time: %u:%u:%u", dataHolder.sunsetTime.hour, dataHolder.sunsetTime.minute, dataHolder.sunsetTime.second);
        }
        else
        {
            ESP_LOGE(TAG, "Parsing failed!");
        }

        sendDataToQueue(&dataHolder);

        udpSocket = udp_getUDPsocket();
        udp_sendMsg(udpSocket);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        udp_recvMsg(udpSocket, &processNTPResult);

        vTaskDelay(30000 / portTICK_PERIOD_MS);
    }
    
    
}