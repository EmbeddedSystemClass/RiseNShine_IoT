/* User GUI over TCP */

#include "gui_user.h"
#include "stepper.h"
#include "esp_log.h"
#include <string.h>

#define MSGBUFFERLENGTHBYTES 512

static const char * TAG = "GUI User";

extern void gui_sendStepperSteps();

static char msgBuffer[MSGBUFFERLENGTHBYTES];
static bool didUserExit;
static uint sendMsgLength;

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
                   "  (1) Set direction (0 = CW, 1 = CCW)\n"
                   "  (2) Stop Stepper                   \n"
                   "  (3) Add stepper steps              \n"
                   "  (4) EXIT                           \n"
                   "=====================================\n";
    gui_copyMsgToSend(menu, sizeof(menu));     
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
    gui_sendStepperSteps(steps);
    char msg[] = "Added stepper steps\n";
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