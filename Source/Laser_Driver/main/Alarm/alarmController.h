#ifndef __ALARM_CONTROLLER_H
#define __ALARM_CONTROLLER_H

#include "alarmController_cfg.h"

/******************************************************************************
*   Public Definitions
*******************************************************************************/


/******************************************************************************
*   Public Macros
*******************************************************************************/


/******************************************************************************
*   Public Data Types
*******************************************************************************/
typedef enum ALARM_State_e{
    ALARM_STATE_INACTIVE,
    ALARM_STATE_ACTIVE,

    ALARM_STATE_INVALID,
}ALARM_State_t;

typedef enum ALARM_Ret_e{
    ALARM_STATUS_ERROR,
    ALARM_STATUS_OK,
}ALARM_Ret_t;

typedef void(*AlarmStatusCallback_t)(ALARM_Src_t src, ALARM_State_t state);
typedef ALARM_State_t(*AlarmMonitoring_t)(ALARM_Src_t src);

/******************************************************************************
*   Public Variables
*******************************************************************************/


/******************************************************************************
*   Error Check
*******************************************************************************/


/******************************************************************************
*   Public Functions Declarations
*******************************************************************************/
ALARM_Ret_t ALARM_InitController(AlarmMonitoring_t monitoring_func,
                                 AlarmStatusCallback_t callback);

ALARM_Ret_t ALARM_GetState(ALARM_Src_t src, ALARM_State_t *pState);


#endif//__ALARM_CONTORLLER_H