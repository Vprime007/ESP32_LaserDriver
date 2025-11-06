#ifndef __TRIGGER_DRIVER_H
#define __TRIGGER_DRIVER_H

#include <stdint.h>

/******************************************************************************
*   Public Definitions
*******************************************************************************/


/******************************************************************************
*   Public Macros
*******************************************************************************/
typedef void(*triggerStateChangeCallback_t)(TRIGGER_State_t state);

/******************************************************************************
*   Public Data Types
*******************************************************************************/
typedef struct TRIGGER_Config_s{
    uint8_t trigger_gpio;
    TRIGGER_Active_Level_t active_level;
}TRIGGER_Config_t;

typedef enum TRIGGER_State_e{
    TRIGGER_STATE_RELEASE,
    TRIGGER_STATE_PRESS,

    TRIGGER_STATE_INVALID,
}TRIGGER_State_t;

typedef enum TRIGGER_Active_level_e{
    TRIGGER_ACTIVE_LEVEL_LOW,
    TRIGGER_ACTIVE_LEVEL_HIGH,
}TRIGGER_Active_Level_t;

typedef enum TRIGGER_Ret_e{
    TRIGGER_STATUS_ERROR,
    TRIGGER_STATUS_OK,
}TRIGGER_Ret_t;

/******************************************************************************
*   Public Variables
*******************************************************************************/


/******************************************************************************
*   Error Check
*******************************************************************************/


/******************************************************************************
*   Public Functions Declarations
*******************************************************************************/
TRIGGER_Ret_t TRIGGER_InitDriver(TRIGGER_Config_t *pConfig,
                                 triggerStateChangeCallback_t callback);

#endif//__TRIGGER_DRIVER_H