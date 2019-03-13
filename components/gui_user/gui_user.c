/* User GUI over TCP */

typedef enum {
    USRCMD_GOTOMAINMENU = 0,
    USRCMD_SETDIRECTION,
    USRCMD_STOPSTEPPER,
    USRCMD_ADDDIRECTION,
    USRCMD_INVALID
} userCommands_e;

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