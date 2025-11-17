#ifndef __LASER_CONTROLLER_H
#define __LASER_CONTROLLER_H

#include <stdint.h>

/******************************************************************************
*   Public Definitions
*******************************************************************************/
#define LASER_MAX_PERCENT               (10000)
#define LASER_MIN_PERCENT               (0)

/******************************************************************************
*   Public Macros
*******************************************************************************/


/******************************************************************************
*   Public Data Types
*******************************************************************************/
typedef enum LASER_State_e{
    LASER_STATE_INACTIVE,
    LASER_STATE_ACTIVE,

    LASER_STATE_INVALID,
}LASER_State_t;

typedef enum LASER_Ret_e{
    LASER_STATUS_ERROR,
    LASER_STATUS_OK,
}LASER_Ret_t;

/******************************************************************************
*   Public Variables
*******************************************************************************/


/******************************************************************************
*   Error Check
*******************************************************************************/


/******************************************************************************
*   Public Functions Declarations
*******************************************************************************/
LASER_Ret_t LASER_InitController(void);

LASER_Ret_t LASER_SetActive(uint16_t percent);

LASER_Ret_t LASER_SetInactive(void);

LASER_Ret_t LASER_GetState(LASER_State_t *pState);


#endif//__LASER_CONTROLLER_H
