/* User GUI over TCP */

#include "gui_user.h"
#include "stepper.h"
#include "esp_log.h"
#include <string.h>

/// @todo: Maybe create a new header primarily to hold queueHandles
#include "FreeRTOS.h"
#include "freertos/queue.h"
#include "risenshine_systemtasks.h" // for stepCmd_e

#include "clock_management.h"

#define MSGBUFFERLENGTHBYTES 512

static const char * TAG = "GUI User";

extern QueueHandle_t qStepperCommand;

static char msgBuffer[MSGBUFFERLENGTHBYTES];
static bool didUserExit;
static uint sendMsgLength;
static const TickType_t queueDelayMs = 500 / portTICK_PERIOD_MS;

typedef struct msgPacket_
{
    userCommands_e cmd;
    int data;
} msgPacket_t;

int gui_showGui(char* msg, uint msgLen, uint bufferLen)
{
    memset(msg, 0, bufferLen);
    return -1;
}

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

static void gui_copyMsgToSend(char* msg, uint len)
{
    memcpy(msgBuffer, msg, len);
    msgBuffer[len] = 0;
    sendMsgLength = len;
}

static void gui_printMenu()
{
    char menu[] =  "=================MENU================\n"
                   "  (0) Print Menu                     \n"
                   "  (1) Open blinds                    \n"
                   "  (2) Close blinds                   \n"
                   "  (3) Get blinds status              \n"
                   "  (4) Get current time stored        \n"
                   "  (5) Get sunrise time stored        \n"
                   "  (6) Get sunset time stored         \n"
                   "  (7) Set direction (0 = CW, 1 = CCW)\n"
                   "  (8) Stop Stepper                   \n"
                   "  (9) Add stepper steps              \n"
                   "=====================================\n";
    gui_copyMsgToSend(menu, sizeof(menu));     
}

static void gui_openBlinds()
{
    char msg[] = "Opening blinds";
    stepCmd_e stepCommand = STEPCMD_OPENBLINDS;
    xQueueSend(qStepperCommands, &stepCommand, queueDelayMs);
    gui_copyMsgToSend(msg, sizeof(msg));
}

static void gui_closeBlinds()
{
    char msg[] = "Closing blinds";
    stepCmd_e stepCommand = STEPCMD_CLOSEBLINDS;
    xQueueSend(qStepperCommands, &stepCommand, queueDelayMs);
    gui_copyMsgToSend(msg, sizeof(msg));
}

static void gui_getBlindsStatus()
{
    bool blindsStatus = true;
    /// @todo Issue # 13: Expose the blinds status
    if (blindsStatus)
    {
        char msg[] = "Blinds is open";
        gui_copyMsgToSend(msg, sizeof(msg));
    }
    else
    {
        char msg[] = "Blinds is closed";
        gui_copyMsgToSend(msg, sizeof(msg));
    }
}

static void gui_getCurrentTime()
{
    char msg[30];
    timeFormat_t currentTime;
    clock_getTime(CLOCK_CURRENTTIME, &currentTime);
    memset(msg, 0, sizeof(msg));
    sprintf(msg, "Current time: %02d:%02d:%02d", currentTime.hour, currentTime.minute, currentTime.second);
    gui_copyMsgToSend(msg, sizeof(msg));
}

static void gui_getSunriseTime()
{
    char msg[30];
    timeFormat_t sunriseTime;
    clock_getTime(CLOCK_SUNRISETIME, &sunriseTime);
    memset(msg, 0, sizeof(msg));
    sprintf(msg, "Sunrise time: %02d:%02d:%02d", sunriseTime.hour, sunriseTime.minute, sunriseTime.second);
    gui_copyMsgToSend(msg, sizeof(msg));
}

static void gui_getSunsetTime()
{
    char msg[30];
    timeFormat_t sunsetTime;
    clock_getTime(CLOCK_SUNSETTIME, &sunsetTime);
    memset(msg, 0, sizeof(msg));
    sprintf(msg, "Sunset time: %02d:%02d:%02d", sunsetTime.hour, sunsetTime.minute, sunsetTime.second);
    gui_copyMsgToSend(msg, sizeof(msg));
}

static void gui_setStepperDirection(int set)
{
    if (set == 0)
    {
        char msg[] = "Stepper set to CLOCKWISE\n";
        gui_copyMsgToSend(msg, sizeof(msg));
        stepper_setStepperDirection(CLOCKWISE);
    }
    else
    {
        char msg[] = "Stepper set to COUNTERCLOCKWISE\n";
        gui_copyMsgToSend(msg, sizeof(msg));
        stepper_setStepperDirection(COUNTCLOCKWISE);
    }
}

static void gui_stopStepper()
{
    stepper_stopStepper();
    char msg[] = "Stepper STOPPED\n";
    gui_copyMsgToSend(msg, sizeof(msg));
}

static void gui_addStepperDirection(int steps)
{
    stepper_moveStepper(steps);
    char msg[30]; 
    sprintf(msg, "Added stepper steps: %d",steps);
    gui_copyMsgToSend(msg, sizeof(msg));
}

static void gui_printInvalidCmd()
{
    char msg[] = "Error: Invalid command.\n";
    gui_copyMsgToSend(msg, sizeof(msg));
}

static void gui_processCmd(msgPacket_t dataPkg)
{
    switch (dataPkg.cmd)
    {
        case USRCMD_GOTOMAINMENU:
            gui_printMenu();
            break;
        case USRCMD_OPENBLINDS:
            gui_openBlinds();
            break;
        case USRCMD_CLOSEBLINDS:
            gui_closeBlinds();
            break;
        case USRCMD_GETBLINDSSTATUS:
            gui_getBlindsStatus();
            break;
        case USRCMD_GETCURRENTTIME:
            gui_getCurrentTime();
            break;
        case USRCMD_GETSUNRISETIME:
            gui_getSunriseTime();
            break;
        case USRCMD_GETSUNSETTIME:
            gui_getSunsetTime();
            break;
        case USRCMD_SETDIRECTION:
            gui_setStepperDirection(dataPkg.data);
            break;
        case USRCMD_STOPSTEPPER:
            gui_stopStepper();
            break;
        case USRCMD_ADDDIRECTION:
            gui_addStepperDirection(dataPkg.data);
            break;
        case USRCMD_EXIT:
            didUserExit = true;
            break;
        default:
            gui_printInvalidCmd();
            break;
    }
}

static void gui_processUserInput(char *msg, uint msgLen)
{
    msgPacket_t recvMsgPacket;
    recvMsgPacket.cmd = (userCommands_e) charToInt(msg, 1);
    if (msgLen > 1)
    {
        recvMsgPacket.data = charToInt(msg+1, msgLen-1);
    }
    else
    {
        recvMsgPacket.cmd = USRCMD_INVALID;
    }

    ESP_LOGI(TAG, "Received command: %d", recvMsgPacket.cmd);
    ESP_LOGI(TAG, "Received data command: %d", recvMsgPacket.data);
    gui_processCmd(recvMsgPacket);
}

/**
 * menuApi as an interface to access the system
 * 
 * @param sendMsg(char*msg, uint len) - callback function to perform the sending of message
 *                                      thru its own protocol. Function returns a bool to
 *                                      check if the send was successful
 * @param recvMsg(char*msg, uint len) - callback function to receive message thru its
 *                                      own protocol. This assumes the message is null-terminated.
 *                                      The function returns the number of bytes it received.
 */
void gui_menuApi(bool(*sendMsg)(int s, char*msg, uint msgLen), int(*recvMsg)(int s, char*msg, uint bufferLen), int s)
{
    int recvMsgInBytes;
    bool sendStatus = false;
    didUserExit = false;

    do
    {
        memset(msgBuffer, 0, sizeof(msgBuffer));
        recvMsgInBytes = (*recvMsg)(s, msgBuffer, sizeof(msgBuffer));
        gui_processUserInput(msgBuffer, recvMsgInBytes);

        sendStatus = (*sendMsg)(s, msgBuffer, sendMsgLength);

    } while (recvMsgInBytes > 0 && sendStatus && !didUserExit);
}