/* User GUI over TCP */

#include "gui_user.h"

void gui_processInput(userCommands_e cmd)
{
    switch (cmd)
    {
        case USRCMD_GOTOMAINMENU:
            break;
        case USRCMD_SETDIRECTION:
            break;
        case USRCMD_STOPSTEPPER:
            break;
        case USRCMD_ADDDIRECTION:
            break;
        case USRCMD_INVALID:
            break;
        default:
            break;
    }
}

void gui_printMenu()
{
    char menu[] =  "=================MENU==============/n"
                   "  (1) OPTION 1                     /n"
                   "  (2) OPTION 2                     /n"
                   "  (3) OPTION 3                     /n"
                   "  (4) OPTION 4                     /n"
                   "  (x) EXIT                         /n"
                   "===================================";     
}