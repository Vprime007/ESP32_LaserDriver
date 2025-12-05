#ifndef __THERMAL_MANAGEMENT_H
#define __THERMAL_MANAGEMENT_H

/******************************************************************************
*   Public Definitions
*******************************************************************************/


/******************************************************************************
*   Public Macros
*******************************************************************************/


/******************************************************************************
*   Public Data Types
*******************************************************************************/
typedef enum THERMAL_Source_e{
    THERMAL_SRC_LOAD_FAN,
    THERMAL_SRC_PHASE_FAN,

    THERMAL_SRC_INVALID,
}THERMAL_Source_t;

typedef enum THERMAL_State_e{
    THERMAL_STATE_DISABLE,
    THERMAL_STATE_ENABLE,

    THERMAL_STATE_INVALID,
}THERMAL_State_t;

typedef enum THERMAL_Ret_e{
    THERMAL_STATUS_ERROR,
    THERMAL_STATUS_OK,
}THERMAL_Ret_t;

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
*  \brief Thermal management initialization.
*
*   This function is used to initialize the thermal management module.
*   
*   Preconditions: None.
*
*   Side Effects: None.
*
*   \return     Operation status
*
*******************************************************************************/
THERMAL_Ret_t THERMAL_InitManager(void);

/***************************************************************************//*!
*  \brief Set thermal management source state.
*
*   This function is used to enable/disable a thermal management source.
*   
*   Preconditions: None.
*
*   Side Effects: None.
*
*   \param[in]  source              Thermal management source.
*   \param[in]  state               Source state (enable/Disable).
*
*   \return     Operation status
*
*******************************************************************************/
THERMAL_Ret_t THERMAL_SetState(THERMAL_Source_t source, THERMAL_State_t state);

/***************************************************************************//*!
*  \brief Get thermal management source state.
*
*   This function is used to get the current state of a thermal 
*   management source.
*   
*   Preconditions: None.
*
*   Side Effects: None.
*
*   \param[in]  source              Thermal management source.
*   \param[in]  pState              Pointer to store source state.
*
*   \return     Operation status
*
*******************************************************************************/
THERMAL_Ret_t THERMAL_GetState(THERMAL_Source_t source, THERMAL_State_t *pState);

#endif//__TEMP_MANAGEMENT_H