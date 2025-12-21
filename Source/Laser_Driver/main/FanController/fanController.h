#ifndef __FAN_CONTROLLER_H
#define __FAN_CONTROLLER_H

/******************************************************************************
*   Public Definitions
*******************************************************************************/


/******************************************************************************
*   Public Macros
*******************************************************************************/


/******************************************************************************
*   Public Data Types
*******************************************************************************/
typedef enum FAN_State_e{
    FAN_STATE_DISABLE,
    FAN_STATE_ENABLE,

    FAN_STATE_INVALID,
}FAN_State_t;

typedef enum FAN_Id_e{
    FAN_ID_LOAD,
    FAN_ID_PHASE,

    FAN_ID_INVALID,
}FAN_Id_t;

typedef enum FAN_Ret_e{
    FAN_STATUS_ERROR,
    FAN_STATUS_OK,
}FAN_Ret_t;

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
*  \brief Fan controller initialization.
*
*   This function is used to initialize the fan controller module.
*   
*   Preconditions: None.
*
*   Side Effects: None.
*
*   \return     Operation status
*
*******************************************************************************/
FAN_Ret_t FAN_InitController(void);

/***************************************************************************//*!
*  \brief Set fan state.
*
*   This function is used to enable/disable a fan.
*   
*   Preconditions: None.
*
*   Side Effects: None.
*
*   \param[in]  fan_id              Target fan ID.
*   \param[in]  state               Fan state (enabel/Disable)
*
*   \return     Operation status
*
*******************************************************************************/
FAN_Ret_t FAN_SetState(FAN_Id_t fan_id, FAN_State_t state);

/***************************************************************************//*!
*  \brief Get fan state.
*
*   This function is used to get the current state of a fan.
*   
*   Preconditions: None.
*
*   Side Effects: None.
*
*   \param[in]  fan_id              Target fan ID.
*   \param[in]  pState              Pointer to store fan state.
*
*   \return     Operation status
*
*******************************************************************************/
FAN_Ret_t FAN_GetState(FAN_Id_t fan_id, FAN_State_t *pState);

#endif//__FAN_CONTROLLER_H