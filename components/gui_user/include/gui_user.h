/* gui_user.h */

#include "esp_system.h"

typedef enum {
    USRCMD_GOTOMAINMENU = 0,
    USRCMD_SETDIRECTION,
    USRCMD_STOPSTEPPER,
    USRCMD_ADDDIRECTION,
    USRCMD_EXIT,
    USRCMD_INVALID
} userCommands_e;

void gui_menuApi(bool(*sendMsg)(int s, char*msg, uint msgLen), int(*recvMsg)(int s, char*msg, uint bufferLen), int s);