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
/***************************************************************************//*!
*  \brief Laser Controller initialization.
*
*   This function perform the initialization of the laser controller module.
*   It also add and managed both laser instances.
*   
*   Preconditions: None.
*
*   Side Effects: None.
*
*   \return         Operation status
*
*******************************************************************************/
LASER_Ret_t LASER_InitController(void);

/***************************************************************************//*!
*  \brief Set laser active percentage.
*
*   This function is used to set the Active state fade duty-cycle in percent.
*   (100% -> 10000). The laser module use a pwm to fade the laser output.
*   The percent value represent the duty-cycle of the fading pwm.
*   
*   Preconditions: None.
*
*   Side Effects: None.
*
*   \param[in]      percent             Laser pwm duty-cycle percent.
*
*   \return         Operation status
*
*******************************************************************************/
LASER_Ret_t LASER_SetActivePercent(uint16_t percent);

/***************************************************************************//*!
*  \brief Set laser active
*
*   This function activate both laser phases and set the pwms duty-cycles to the
*   active percent value. 
*   
*   Preconditions: None.
*
*   Side Effects: None.
*
*   \return         Operation status
*
*******************************************************************************/
LASER_Ret_t LASER_SetActive(void);

/***************************************************************************//*!
*  \brief Set laser inactive.
*
*   This function disable both laser phases and set the pwms duty-cycles to the
*   minimum allowed.
*   
*   Preconditions: None.
*
*   Side Effects: None.
*
*   \return         Operation status
*
*******************************************************************************/
LASER_Ret_t LASER_SetInactive(void);

/***************************************************************************//*!
*  \brief Get laser state.
*
*   This function is used to get the current lasers state (Active/Inactive).
*   
*   Preconditions: None.
*
*   Side Effects: None.
*
*   \param[out]     pState                  Pointer to store state.
*
*   \return         Operation status
*
*******************************************************************************/
LASER_Ret_t LASER_GetState(LASER_State_t *pState);

/***************************************************************************//*!
*  \brief Get Active persent.
*
*   This function is used to get the current active percent pwm values.
*   (100% -> 10000)
*   
*   Preconditions: None.
*
*   Side Effects: None.
*
*   \param[out]     pState                  Pointer to store the percent.
*
*   \return         Operation status
*
*******************************************************************************/
LASER_Ret_t LASER_GetActivePercent(uint16_t *pPercent);


#endif//__LASER_CONTROLLER_H
