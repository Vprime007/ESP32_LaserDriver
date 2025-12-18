/******************************************************************************
*   Includes
*******************************************************************************/
#include <string.h>
#include <stdio.h>

#include "thermalManagement.h"
#include "laserController.h"
#include "alarmController.h"
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
static int32_t shellEnableAllPhase(int32_t argc, char *argv[]);
static int32_t shellDisableAllPhase(int32_t argc, char *argv[]);
static int32_t shellEnablePhase(int32_t argc, char *argv[]);
static int32_t shellDisablePhase(int32_t argc, char *argv[]);
static int32_t shellSetPhaseOverTemp(int32_t argc, char *argv[]);
static int32_t shellSetLoadOverTemp(int32_t argc, char *argv[]);
static int32_t shellSetUnderVolt(int32_t argc, char *argv[]);
static int32_t shellGetOverTemp(int32_t argc, char *argv[]);
static int32_t shellGetUnderVolt(int32_t argc, char *argv[]);
static int32_t shellGetTemperature(int32_t argc, char *argv[]);
static int32_t shellGetPwr(int32_t argc, char *argv[]);
static int32_t shellEnableLoadFan(int32_t argc, char *argv[]);
static int32_t shellDisableLoadFan(int32_t argc, char *argv[]);
static int32_t shellEnablePhaseFan(int32_t argc, char *argv[]);
static int32_t shellDisablePhaseFan(int32_t argc, char *argv[]);

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
    {"enable-all-phase", shellEnableAllPhase, "Enable All phases outputs"},
    {"disable-all-phase", shellDisableAllPhase, "Disable All phases outputs"},
    {"enable-phase", shellEnablePhase, "Enable phase output"},
    {"disable-phase", shellDisablePhase, "Disable phase output"},
    {"set-phase-ovt", shellSetPhaseOverTemp, "Set phases OVT threshold (in 10m*C)"},
    {"set-load_ovt", shellSetLoadOverTemp, "Set load OVT threshold (in 10m*C)"},
    {"set-under-volt", shellSetUnderVolt, "Set Under Voltage threshold (in 10mV)"},
    {"get-ovt", shellGetOverTemp, "Get OVT threshold (in 10m*C)"},
    {"get-uvl", shellGetUnderVolt, "Get Under voltage thershold (in 10mV)"},
    {"get-temp", shellGetTemperature, "Get temperature values (in *C)"},
    {"get-pwr", shellGetPwr, "Get pwr measurements"},
    {"enable-load-fan", shellEnableLoadFan, "Enable the Load fan"},
    {"disable-load-fan", shellDisableLoadFan, "Disable the load fan"},
    {"enable-phases-fan", shellEnablePhaseFan, "Enable the Phases fan"},
    {"disable-phases-fan", shellDisablePhaseFan, "Disable the Phases fan"},
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
*  \brief   Shell enable all phases
*
*   Shell cmd to enable all phases.
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
static int32_t shellEnableAllPhase(int32_t argc, char *argv[]){

    if(LASER_STATUS_OK != LASER_SetAllPhaseActive()){
        
        shellResultError(SHELL_ERR_PARAMS);
        return SHELL_ERR_PARAMS;
    }

    shellResultSuccess();

    return SHELL_ERR_NO_ERROR;
}

/***************************************************************************//*!
*  \brief   Shell disable all phases
*
*   Shell cmd to disable all phases.
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
static int32_t shellDisableAllPhase(int32_t argc, char *argv[]){

    if(LASER_STATUS_OK != LASER_SetAllPhaseInactive()){

        shellResultError(SHELL_ERR_PARAMS);
        return SHELL_ERR_PARAMS;
    }

    shellResultSuccess();

    return SHELL_ERR_NO_ERROR;
}

/***************************************************************************//*!
*  \brief   Shell enable phase
*
*   Shell cmd to enable target phase.
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

    if(argc < 2){

        shellResultError(SHELL_ERR_PAYLOAD);
        return SHELL_ERR_PAYLOAD;
    }

    if((0 == strcmp("PHASE_A", argv[1])) || (0 == strcmp("phase_a", argv[1]))){
        
        if(LASER_STATUS_OK != LASER_SetPhaseActive(LASER_PHASE_A)){

            shellResultError(SHELL_ERR_PARAMS);
            return SHELL_ERR_PARAMS;
        }
    }
    else if((0 == strcmp("PHASE_B", argv[1])) || (0 == strcmp("phase_b", argv[1]))){

        if(LASER_STATUS_OK != LASER_SetPhaseActive(LASER_PHASE_B)){

            shellResultError(SHELL_ERR_PARAMS);
            return SHELL_ERR_PARAMS;
        }
    }
    else{

        shellResultError(SHELL_ERR_PARAMS);
        return SHELL_ERR_PARAMS;
    }

    shellResultSuccess();

    return SHELL_ERR_NO_ERROR;
}

/***************************************************************************//*!
*  \brief   Shell disable phase
*
*   Shell cmd to disable target phase.
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

    if(argc < 2){

        shellResultError(SHELL_ERR_PAYLOAD);
        return SHELL_ERR_PAYLOAD;
    }

    if((0 == strcmp("PHASE_A", argv[1])) || (0 == strcmp("phase_a", argv[1]))){

        if(LASER_STATUS_OK != LASER_SetPhaseInactive(LASER_PHASE_A)){

            shellResultError(SHELL_ERR_PARAMS);
            return SHELL_ERR_PARAMS;
        }
    }
    else if((0 == strcmp("PHASE_B", argv[1])) || (0 == strcmp("phase_b", argv[1]))){

        if(LASER_STATUS_OK != LASER_SetPhaseInactive(LASER_PHASE_B)){

            shellResultError(SHELL_ERR_PARAMS);
            return SHELL_ERR_PARAMS;
        }
    }
    else{

        shellResultError(SHELL_ERR_PARAMS);
        return SHELL_ERR_PARAMS;
    }

    return SHELL_ERR_NO_ERROR;
}

/***************************************************************************//*!
*  \brief   Shell set phase Over temperature.
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
static int32_t shellSetPhaseOverTemp(int32_t argc, char *argv[]){

    if(argc != 3){
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

    //make sure that release is lower than threshold
    if(temp_threshold < temp_release){

        shellResultError(SHELL_ERR_PARAMS);
        return SHELL_ERR_PARAMS;
    }

    ALARM_Temp_Config_t temp_cfg = {
        .temp_threshold_10mC = temp_threshold,
        .temp_release_10mc = temp_release,
    };
    if(ALARM_STATUS_OK != ALARM_SetTempLevel(ALARM_SRC_PHASE_A_TEMP, &temp_cfg)){

        shellResultError(SHELL_ERR_PARAMS);
        return SHELL_ERR_PARAMS;
    }

    shellResultSuccess();

    return SHELL_ERR_NO_ERROR;
}

static int32_t shellSetLoadOverTemp(int32_t argc, char *argv[]){

    if(argc != 3){
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

    //make sure that release is lower than threshold
    if(temp_threshold < temp_release){

        shellResultError(SHELL_ERR_PARAMS);
        return SHELL_ERR_PARAMS;
    }

    ALARM_Temp_Config_t temp_cfg = {
        .temp_threshold_10mC = temp_threshold,
        .temp_release_10mc = temp_release,
    };
    if(ALARM_STATUS_OK != ALARM_SetTempLevel(ALARM_SRC_LOAD_TEMP, &temp_cfg)){

        shellResultError(SHELL_ERR_PARAMS);
        return SHELL_ERR_PARAMS;
    }

    shellResultSuccess();

    return SHELL_ERR_NO_ERROR;
}

/***************************************************************************//*!
*  \brief   Shell set under voltage.
*
*   Shell cmd to set under volatge threshold.
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
static int32_t shellSetUnderVolt(int32_t argc, char *argv[]){

    if(argc < 3){

        shellResultError(SHELL_ERR_PAYLOAD);
        return SHELL_ERR_PAYLOAD;
    }

    int16_t volt_threshold = strtoul(argv[1], NULL, 10);
    int16_t volt_release = strtoul(argv[2], NULL, 10);

    //Check if within values are within ranges
    if(((volt_threshold < 2000) || (volt_threshold > 9000)) || 
       ((volt_release < 2000) || (volt_release > 9000))){

        shellResultError(SHELL_ERR_PARAMS);
        return SHELL_ERR_PARAMS;
    }

    //Make sure that release is greater than threshold
    if(volt_threshold > volt_release){

        shellResultError(SHELL_ERR_PARAMS);
        return SHELL_ERR_PARAMS;
    }

    ALARM_Volt_Config_t volt_cfg = {
        .volt_threshold_10mv = volt_threshold,
        .volt_release_10mv = volt_release, 
    };
    if(ALARM_STATUS_OK != ALARM_SetVoltLevel(&volt_cfg)){

        shellResultError(SHELL_ERR_PARAMS);
        return SHELL_ERR_PARAMS;
    }

    shellResultSuccess();

    return SHELL_ERR_NO_ERROR;
}

/***************************************************************************//*!
*  \brief   Shell get Over temperature threshold.
*
*   Shell cmd to get over temperature threshold.
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
static int32_t shellGetOverTemp(int32_t argc, char *argv[]){

    ALARM_Temp_Config_t phase_cfg;
    ALARM_Temp_Config_t load_cfg;

    if(ALARM_STATUS_OK != ALARM_GetTempLevel(ALARM_SRC_LOAD_TEMP, &load_cfg)){
        shellResultError(SHELL_ERR_PARAMS);
        return SHELL_ERR_PARAMS;
    }

    if(ALARM_STATUS_OK != ALARM_GetTempLevel(ALARM_SRC_PHASE_A_TEMP, &phase_cfg)){
        shellResultError(SHELL_ERR_PARAMS);
        return SHELL_ERR_PARAMS;
    }

    char response_buf[128] = {0};
    snprintf(response_buf, sizeof(response_buf), "%s Phase Thres: %d - Rel: %d / Load Thres: %d - Rel: %d",
                                                 SHELL_RESPONSE_INIT,
                                                 phase_cfg.temp_threshold_10mC,
                                                 phase_cfg.temp_release_10mc,
                                                 load_cfg.temp_threshold_10mC,
                                                 load_cfg.temp_release_10mc);

    SHELL_PutLine(response_buf);
    return SHELL_ERR_NO_ERROR;
}

/***************************************************************************//*!
*  \brief   Shell get Under voltage threshold.
*
*   Shell cmd to get under voltage threshold.
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
static int32_t shellGetUnderVolt(int32_t argc, char *argv[]){

    ALARM_Volt_Config_t volt_cfg;

    if(ALARM_STATUS_OK != ALARM_GetVoltLevel(&volt_cfg)){
        shellResultError(SHELL_ERR_PARAMS);
        return SHELL_ERR_PARAMS;
    }

    char response_buf[64] = {0};
    snprintf(response_buf, sizeof(response_buf), "%s Threshold: %d / Release: %d",
                                                 SHELL_RESPONSE_INIT,
                                                 volt_cfg.volt_threshold_10mv,
                                                 volt_cfg.volt_release_10mv);

    SHELL_PutLine(response_buf);
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
*  \brief   Shell enable load fan.
*
*   Shell cmd to enable load fan.
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
static int32_t shellEnableLoadFan(int32_t argc, char *argv[]){

    if(THERMAL_STATUS_OK != THERMAL_SetState(THERMAL_SRC_LOAD_FAN, THERMAL_STATE_ENABLE)){

        shellResultError(SHELL_ERR_PARAMS);
        return SHELL_ERR_PARAMS;
    }

    shellResultSuccess();
    return SHELL_ERR_NO_ERROR;
}

/***************************************************************************//*!
*  \brief   Shell disable load fan.
*
*   Shell cmd to disable load fan.
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
static int32_t shellDisableLoadFan(int32_t argc, char *argv[]){

    if(THERMAL_STATUS_OK != THERMAL_SetState(THERMAL_SRC_LOAD_FAN, THERMAL_STATE_DISABLE)){

        shellResultError(SHELL_ERR_PARAMS);
        return SHELL_ERR_PARAMS;
    }

    shellResultSuccess();
    return SHELL_ERR_NO_ERROR;
}

/***************************************************************************//*!
*  \brief   Shell enable phases fan.
*
*   Shell cmd to enable phases fan.
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
static int32_t shellEnablePhaseFan(int32_t argc, char *argv[]){

    if(THERMAL_STATUS_OK != THERMAL_SetState(THERMAL_SRC_PHASE_FAN, THERMAL_STATE_ENABLE)){

        shellResultError(SHELL_ERR_PARAMS);
        return SHELL_ERR_PARAMS;
    }

    shellResultSuccess();
    return SHELL_ERR_NO_ERROR;
}

/***************************************************************************//*!
*  \brief   Shell disable phases fan.
*
*   Shell cmd to disable phases fan.
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
static int32_t shellDisablePhaseFan(int32_t argc, char *argv[]){

    if(THERMAL_STATUS_OK != THERMAL_SetState(THERMAL_SRC_PHASE_FAN, THERMAL_STATE_DISABLE)){

        shellResultError(SHELL_ERR_PARAMS);
        return SHELL_ERR_PARAMS;
    }

    shellResultSuccess();
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


