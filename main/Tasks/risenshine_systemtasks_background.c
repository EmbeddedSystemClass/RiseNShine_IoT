/**
 * risenshine_systemtasks_background
 * 
 * Task implementation for low-priority tasks
 */

#include "risenshine_systemtasks.h"

#include "http_get.h"

#include "esp_log.h"

static const char * TAG = "Background task";

#define PAYLOADSIZE 1024

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

void vTaskIdleComputations(void *pvParameters)
{
    char payload[PAYLOADSIZE];
    char * htmlContent = NULL;
    int socket = -1;
    strSunriseSunsetTimes_t dataHolder;
    bool parseSuccess = false;
 
    while(1)
    {
        socket = http_connectSocket();
        http_sendRequest(socket, payload, PAYLOADSIZE);

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
            ESP_LOGI(TAG, "Sunrise time: %u:%u:%u", dataHolder.sunriseTime.hour, dataHolder.sunriseTime.minute, dataHolder.sunriseTime.second);
            ESP_LOGI(TAG, "Sunset time: %u:%u:%u", dataHolder.sunsetTime.hour, dataHolder.sunsetTime.minute, dataHolder.sunsetTime.second);
        }
        else
        {
            ESP_LOGE(TAG, "Parsing failed!");
        }

        vTaskDelay(30000 / portTICK_PERIOD_MS);
    }
    
    
}