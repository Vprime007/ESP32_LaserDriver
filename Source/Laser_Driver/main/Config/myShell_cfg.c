/******************************************************************************
*   Includes
*******************************************************************************/
#include <string.h>
#include <stdio.h>


#include "laserController.h"
#include "myShell_cfg.h"

/******************************************************************************
*   Private Definitions
*******************************************************************************/
#define ARRAY_SIZE(arr)                 (sizeof(arr) / sizeof(arr[0]))

#define SHELL_RESPONSE_INIT             (">")
#define SHELL_RESPONSE_OK               ("OK")
#define SHELL_RESPONSE_ERR              ("ERROR")
#define SHELL_ERR_PAYLOAD               (255)
#define SHELL_ERR_PARAMS                (254)  
#define SHELL_ERR_NO_ERROR              (0)

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
static int32_t shellSetLoadOvt(int32_t argc, char *argv[]);
static int32_t shellGetTemperature(int32_t argc, char *argv[]);
static int32_t shellGetPwr(int32_t argc, char *argv[]);

static void shellResultSuccess(void);
static void shellResultError(int32_t error);

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
    {"set-dimming", shellSetPhaseActiveDuty, "Set ON duty-cyle (in percent)"},
    {"enable-phase", shellEnablePhase, "Enable phases outputs"},
    {"disable-phase", shellDisablePhase, "Disable phases outputs"},
    {"set-phase-ovt", shellSetPhaseOvt, "Set phases OVT threshold (in *C)"},
    {"set-load-ovt", shellSetLoadOvt, "Set Load OVT threshold (in *C)"},
    {"get-temp", shellGetTemperature, "Get temperature values (in *C)"},
    {"get-pwr", shellGetPwr, "Get pwr measurements"},
};

static uint32_t nb_shell_cmd = ARRAY_SIZE(shell_cmd_table);

/******************************************************************************
*   Private Functions Definitions
*******************************************************************************/
/***************************************************************************//*!
*  \brief   Shell Set phase active duty.
*
*   Shell cmd to set Phase active duty cycle.
*   
*   Preconditions: None.
*
*   Side Effects: None.
*
*   \param[in]  argc                    Number of arguments.
*   \param[in]  argv                    Arguments tables.
*
*   \return     Operation status
*
*******************************************************************************/
static int32_t shellSetPhaseActiveDuty(int32_t argc, char *argv[]){

    if(argc < 2){

        shellResultError(SHELL_ERR_PAYLOAD);
        return SHELL_ERR_PAYLOAD;
    }
    
    uint16_t active_duty = strtoul(argv[1], NULL, 10);
    if(active_duty > 10000){

        shellResultError(SHELL_ERR_PARAMS);
        return SHELL_ERR_PARAMS;
    }

    if(LASER_STATUS_OK != LASER_SetActivePercent(active_duty)){

        shellResultError(SHELL_ERR_PARAMS);
        return SHELL_ERR_PARAMS;
    }

    shellResultSuccess();

    return SHELL_ERR_NO_ERROR;
}

/***************************************************************************//*!
*  \brief   Shell enable phase
*
*   Shell cmd to enable Phases.
*   
*   Preconditions: None.
*
*   Side Effects: None.
*
*   \param[in]  argc                    Number of arguments.
*   \param[in]  argv                    Arguments tables.
*
*   \return     Operation status
*
*******************************************************************************/
static int32_t shellEnablePhase(int32_t argc, char *argv[]){

    if(LASER_STATUS_OK != LASER_SetAllPhaseActive()){
        
        shellResultError(SHELL_ERR_PARAMS);
        return SHELL_ERR_PARAMS;
    }

    shellResultSuccess();

    return SHELL_ERR_NO_ERROR;
}

/***************************************************************************//*!
*  \brief   Shell disable phase
*
*   Shell cmd to disable Phases.
*   
*   Preconditions: None.
*
*   Side Effects: None.
*
*   \param[in]  argc                    Number of arguments.
*   \param[in]  argv                    Arguments tables.
*
*   \return     Operation status
*
*******************************************************************************/
static int32_t shellDisablePhase(int32_t argc, char *argv[]){

    if(LASER_STATUS_OK != LASER_SetAllPhaseInactive()){

        shellResultError(SHELL_ERR_PARAMS);
        return SHELL_ERR_PARAMS;
    }

    shellResultSuccess();

    return SHELL_ERR_NO_ERROR;
}

/***************************************************************************//*!
*  \brief   Shell set phase OVT
*
*   Shell cmd to set phase over temperature threshold.
*   
*   Preconditions: None.
*
*   Side Effects: None.
*
*   \param[in]  argc                    Number of arguments.
*   \param[in]  argv                    Arguments tables.
*
*   \return     Operation status
*
*******************************************************************************/
static int32_t shellSetPhaseOvt(int32_t argc, char *argv[]){

    if(argc < 3){

        shellResultError(SHELL_ERR_PAYLOAD);
        return SHELL_ERR_PAYLOAD;
    }

    int16_t temp_threshold = strtoul(argv[1], NULL, 10);
    int16_t temp_release = strtoul(argv[2], NULL, 10);

    //Check if within values are within ranges
    if(((temp_threshold < 2000) || (temp_threshold > 9000)) || 
       ((temp_release < 2000) || (temp_release > 9000))){

        shellResultError(SHELL_ERR_PARAMS);
        return SHELL_ERR_PARAMS;
    }

    //Check if threshold is greater than release
    if(temp_threshold < temp_release){

        shellResultError(SHELL_ERR_PARAMS);
        return SHELL_ERR_PARAMS;
    }

    shellResultSuccess();

    return SHELL_ERR_NO_ERROR;
}

/***************************************************************************//*!
*  \brief   Shell set load OVT
*
*   Shell cmd to set load over temperature threshold.
*   
*   Preconditions: None.
*
*   Side Effects: None.
*
*   \param[in]  argc                    Number of arguments.
*   \param[in]  argv                    Arguments tables.
*
*   \return     Operation status
*
*******************************************************************************/
static int32_t shellSetLoadOvt(int32_t argc, char *argv[]){

    if(argc < 3){

        shellResultError(SHELL_ERR_PAYLOAD);
        return SHELL_ERR_PAYLOAD;
    }

    int16_t temp_threshold = strtoul(argv[1], NULL, 10);
    int16_t temp_release = strtoul(argv[2], NULL, 10);

    //Check if within values are within ranges
    if(((temp_threshold < 2000) || (temp_threshold > 9000)) || 
       ((temp_release < 2000) || (temp_release > 9000))){

        shellResultError(SHELL_ERR_PARAMS);
        return SHELL_ERR_PARAMS;
    }

    //Check if threshold is greater than release
    if(temp_threshold < temp_release){

        shellResultError(SHELL_ERR_PARAMS);
        return SHELL_ERR_PARAMS;
    }

    shellResultSuccess();

    return SHELL_ERR_NO_ERROR;
}

/***************************************************************************//*!
*  \brief   Shell get temperatures
*
*   Shell cmd to get temperatures.
*   
*   Preconditions: None.
*
*   Side Effects: None.
*
*   \param[in]  argc                    Number of arguments.
*   \param[in]  argv                    Arguments tables.
*
*   \return     Operation status
*
*******************************************************************************/
static int32_t shellGetTemperature(int32_t argc, char *argv[]){


    return SHELL_ERR_NO_ERROR;
}

/***************************************************************************//*!
*  \brief   Shell get power measurements
*
*   Shell cmd to get power measurements.
*   
*   Preconditions: None.
*
*   Side Effects: None.
*
*   \param[in]  argc                    Number of arguments.
*   \param[in]  argv                    Arguments tables.
*
*   \return     Operation status
*
*******************************************************************************/
static int32_t shellGetPwr(int32_t argc, char *argv[]){

    return SHELL_ERR_NO_ERROR;
}

/***************************************************************************//*!
*  \brief   Shell result success
*
*   Shell return successful cmd.
*   
*   Preconditions: None.
*
*   Side Effects: None.
*
*   \return     None.
*
*******************************************************************************/
static void shellResultSuccess(void){

    char success_buf[16] = {0};

    snprintf(success_buf, sizeof(success_buf), "%s %s", SHELL_RESPONSE_INIT, 
                                                        SHELL_RESPONSE_OK);

    SHELL_PutLine(success_buf);
}

/***************************************************************************//*!
*  \brief   Shell result error
*
*   Shell return error cmd.
*   
*   Preconditions: None.
*
*   Side Effects: None.
*
*   \param[in]  error       Error type.
*
*   \return     None.
*
*******************************************************************************/
static void shellResultError(int32_t error){

    char error_buf[32] = {0};

    snprintf(error_buf, sizeof(error_buf), "%s %s: %d", SHELL_RESPONSE_INIT, 
                                                        SHELL_RESPONSE_ERR, 
                                                        error);

    SHELL_PutLine(error_buf);
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


