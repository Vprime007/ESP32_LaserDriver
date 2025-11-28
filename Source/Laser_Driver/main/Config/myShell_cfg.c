/******************************************************************************
*   Includes
*******************************************************************************/
#include <string.h>
#include "myShell_cfg.h"

/******************************************************************************
*   Private Definitions
*******************************************************************************/
#define ARRAY_SIZE(arr)                 (sizeof(arr) / sizeof(arr[0]))

/******************************************************************************
*   Private Macros
*******************************************************************************/


/******************************************************************************
*   Private Data Types
*******************************************************************************/


/******************************************************************************
*   Private Functions Declaration
*******************************************************************************/
static int32_t shellSetPhaseActiveDuty(int32_t argc, char *argv[]);
static int32_t shellSetPhaseInactiveDuty(int32_t argc, char *argv[]);
static int32_t shellEnablePhase(int32_t argc, char *argv[]);
static int32_t shellDisablePhase(int32_t argc, char *argv[]);
static int32_t shellSetPhaseOvt(int32_t argc, char *argv[]);
static int32_t shellSetPhaseOvtRelease(int32_t argc, char *argv[]);
static int32_t shellSetLoadOvt(int32_t argc, char *argv[]);
static int32_t shellSetLoadOvtRelease(int32_t argc, char * argv[]);

static int32_t shellGetTemperature(int32_t argc, char *argv[]);
static int32_t shellGetPwr(int32_t argc, char *argv[]);

/******************************************************************************
*   Public Variables
*******************************************************************************/


/******************************************************************************
*   Private Variables
*******************************************************************************/
//Add shell commands inside the the shell_cmd_table 
//The table should minimally contain the 'help' function.
static SHELL_Command_t shell_cmd_table[] = {
    {"help", SHELL_HelpHandler, "Lists all commands"},
    {"set-active-duty", shellSetPhaseActiveDuty, "Set ON duty-cyle (in percent)"},
    {"set-idle-duty", shellSetPhaseInactiveDuty, "Set OFF duty-cycl (in percent)"},
    {"enable-phase", shellEnablePhase, "Enable phases outputs"},
    {"disable-phase", shellDisablePhase, "Disable phases outputs"},
    {"set-phase-ovt-thr", shellSetPhaseOvt, "Set phases OVT threshold (in *C)"},
    {"set-phase-ovt-rel", shellSetPhaseOvtRelease, "Set phases OVT release (in *C)"},
    {"set-load-ovt-thr", shellSetLoadOvt, "Set Load OVT threshold (in *C)"},
    {"set-load-ovt-rel", shellSetLoadOvtRelease, "Set Load OVT release (in *C)"},
    {"get-temp", shellGetTemperature, "Get temperature values (in *C)"},
    {"get-pwr", shellGetPwr, "Get pwr measurements"},
};

static uint32_t nb_shell_cmd = ARRAY_SIZE(shell_cmd_table);

/******************************************************************************
*   Private Functions Definitions
*******************************************************************************/
static int32_t shellSetPhaseActiveDuty(int32_t argc, char *argv[]){

    return 0;
}

static int32_t shellSetPhaseInactiveDuty(int32_t argc, char *argv[]){

    return 0;
}

static int32_t shellEnablePhase(int32_t argc, char *argv[]){

    return 0;
}

static int32_t shellDisablePhase(int32_t argc, char *argv[]){

    return 0;
}

static int32_t shellSetPhaseOvt(int32_t argc, char *argv[]){

    return 0;
}

static int32_t shellSetPhaseOvtRelease(int32_t argc, char *argv[]){

    return 0;
}

static int32_t shellSetLoadOvt(int32_t argc, char *argv[]){

    return 0;
}

static int32_t shellSetLoadOvtRelease(int32_t argc, char *argv[]){

    return 0;
}

static int32_t shellGetTemperature(int32_t argc, char *argv[]){

    return 0;
}

static int32_t shellGetPwr(int32_t argc, char *argv[]){

    return 0;
}

/******************************************************************************
*   Public Functions Definitions
*******************************************************************************/
/***************************************************************************//*!
*  \brief Get Shell commands table.
*
*   This function return a structure containing a pointer to the Shell 
*   commands table and the size of the table.
*   
*   Preconditions: None.
*
*   Side Effects: None.
*
*   \return     Shell command table infos.
*
*******************************************************************************/
SHELL_Commands_Context_t SHELL_CFG_GetCommandTable(void){

    return (SHELL_Commands_Context_t){.nb_command = nb_shell_cmd, .pTable = shell_cmd_table};
}


/******************************************************************************
*   Interrupts
*******************************************************************************/


