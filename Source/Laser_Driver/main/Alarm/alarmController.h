#ifndef __ALARM_CONTROLLER_H
#define __ALARM_CONTROLLER_H

#include <stdint.h>

/******************************************************************************
*   Public Definitions
*******************************************************************************/
#define ALARM_MONITORING_PERIOD_MS                  (250)

/******************************************************************************
*   Public Macros
*******************************************************************************/


/******************************************************************************
*   Public Data Types
*******************************************************************************/
typedef enum ALARM_Src_e{
    ALARM_SRC_LOAD_TEMP,
    ALARM_SRC_PHASE_A_TEMP,
    ALARM_SRC_PHASE_B_TEMP,

    ALARM_SRC_VBUS,

    ALARM_NUMBER_SRC,
    ALARM_SRC_INVALID = ALARM_NUMBER_SRC,
}ALARM_Src_t;


typedef struct ALARM_Temp_Config_s{
    int16_t temp_threshold_10mC;
    int16_t temp_release_10mc;
}ALARM_Temp_Config_t;

typedef struct ALARM_Volt_Config_s{
    int16_t volt_threshold_10mv;
    int16_t volt_release_10mv;
}ALARM_Volt_Config_t;

typedef enum ALARM_State_e{
    ALARM_STATE_INACTIVE,
    ALARM_STATE_ACTIVE,

    ALARM_STATE_INVALID,
}ALARM_State_t;

typedef enum ALARM_Ret_e{
    ALARM_STATUS_ERROR,
    ALARM_STATUS_OK,
}ALARM_Ret_t;

typedef void(*AlarmStateCallback_t)(ALARM_Src_t src, ALARM_State_t state);

/******************************************************************************
*   Public Variables
*******************************************************************************/


/******************************************************************************
*   Error Check
*******************************************************************************/


/******************************************************************************
*   Public Functions Declarations
*******************************************************************************/
/***************************************************************************//*!
*  \brief Alarm Controller initialization.
*
*   This function is used to initialize the alarm controller module.
*   The callback function will be called when a alarm state change occur.
*   
*   Preconditions: None.
*
*   Side Effects: None.
*
*   \param[in]  pTemp_cfg           Pointer to the temp configuration.
*   \param[in]  pVolt_cfg           Pointer to the volt configuration.
*   \param[in]  callback            Alarm state change callback.
*
*   \return     Operation status
*
*******************************************************************************/
ALARM_Ret_t ALARM_InitController(ALARM_Temp_Config_t *pTemp_cfg,
                                 ALARM_Volt_Config_t *pVolt_cfg,
                                 AlarmStateCallback_t callback);

/***************************************************************************//*!
*  \brief Get alarm state.
*
*   This function is used to get the current state of an alarm.
*   
*   Preconditions: None.
*
*   Side Effects: None.
*
*   \param[in]  src                 Alarm source.
*   \param[out] pState              Pointer to store the state.
*
*   \return     Operation status
*
*******************************************************************************/
ALARM_Ret_t ALARM_GetState(ALARM_Src_t src, ALARM_State_t *pState);

/***************************************************************************//*!
*  \brief Set temperature alarm level.
*
*   This function is used to set the temperature threshold and 
*   release level.
*   
*   Preconditions: None.
*
*   Side Effects: None.
*
*   \param[in]  pTemp_cfg           Pointer to the temperature config.
*
*   \return     Operation status
*
*******************************************************************************/
ALARM_Ret_t ALARM_SetTempLevel(ALARM_Temp_Config_t *pTemp_cfg);

/***************************************************************************//*!
*  \brief Get temperature alarm level.
*
*   This function is used to get the temperature threshold and 
*   release level.
*   
*   Preconditions: None.
*
*   Side Effects: None.
*
*   \param[in]  pTemp_cfg           Pointer to store the temperature config.
*
*   \return     Operation status
*
*******************************************************************************/
ALARM_Ret_t ALARM_GetTempLevel(ALARM_Temp_Config_t *pTemp_cfg);

/***************************************************************************//*!
*  \brief Set voltage level.
*
*   This function is used to set the voltage threshold and 
*   release level.
*   
*   Preconditions: None.
*
*   Side Effects: None.
*
*   \param[in]  pTemp_cfg           Pointer to the voltage config.
*
*   \return     Operation status
*
*******************************************************************************/
ALARM_Ret_t ALARM_SetVoltLevel(ALARM_Volt_Config_t *pVolt_cfg);

/***************************************************************************//*!
*  \brief Get voltage alarm level.
*
*   This function is used to get the voltage threshold and 
*   release level.
*   
*   Preconditions: None.
*
*   Side Effects: None.
*
*   \param[in]  pTemp_cfg           Pointer to store the voltage config.
*
*   \return     Operation status
*
*******************************************************************************/
ALARM_Ret_t ALARM_GetVoltLevel(ALARM_Volt_Config_t *pVolt_cfg);

#endif//__ALARM_CONTORLLER_H