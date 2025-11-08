#ifndef __PWR_MONITORING_H
#define __PWR_MONITORING_H

#include <stdint.h>

/******************************************************************************
*   Public Definitions
*******************************************************************************/
#define PWR_INVALID_VOLTAGE                     (0x8000)
#define PWR_INVALID_CURRENT                     (0x8000)

/******************************************************************************
*   Public Macros
*******************************************************************************/


/******************************************************************************
*   Public Data Types
*******************************************************************************/
typedef enum PWR_Ret_e{
    PWR_MONITORING_STATUS_ERROR,
    PWR_MONITORING_STATUS_OK,
}PWR_Ret_t;

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
*  \brief Power Monitoring initialization.
*
*   This function is used to perform the initialization of the power
*   monitoring module.
*   
*   Preconditions: None.
*
*   Side Effects: None.
*
*   \return     Operation status
*
*******************************************************************************/
PWR_Ret_t PWR_InitMonitoring(void);

/***************************************************************************//*!
*  \brief Process Raw power measurements.
*
*   This function is used to process raw power measurement from the 
*   ADC
*   
*   Preconditions: None.
*
*   Side Effects: None.
*
*   \param[in]  pMeas_buf           Raw measurements buffer.
*   \param[in]  size                Buffer size (in sample)   
*
*   \return     Operation status
*
*******************************************************************************/
PWR_Ret_t PWR_ProcessRawMeasurement(uint8_t *pMeas_buf, uint32_t size);

/***************************************************************************//*!
*  \brief Get bus voltage.
*
*   This function is used to get the latest bus voltage value.
*   In 10mv (50V -> 5000).
*   
*   Preconditions: None.
*
*   Side Effects: None.
*
*   \param[in]  pVoltage_10mv           Pointer to store the voltage value.
*
*   \return     Operation status
*
*******************************************************************************/
PWR_Ret_t PWR_GetBusVoltage(int16_t *pVoltage_10mv);

/***************************************************************************//*!
*  \brief Get Phase A current.
*
*   This function is used to get the latest phase A current.
*   In 10mA (5A -> 500).
*   
*   Preconditions: None.
*
*   Side Effects: None.
*
*   \param[in]  pCurrent_10ma           Pointer to store the current value.
*
*   \return     Operation status
*
*******************************************************************************/
PWR_Ret_t PWR_GetPhaseACurrent(int16_t *pCurrent_10ma);

/***************************************************************************//*!
*  \brief Get Phase A current.
*
*   This function is used to get the latest phase B current.
*   In 10mA (5A -> 500).
*   
*   Preconditions: None.
*
*   Side Effects: None.
*
*   \param[in]  pCurrent_10ma           Pointer to store the current value.
*
*   \return     Operation status
*
*******************************************************************************/
PWR_Ret_t PWR_GetPhaseBCurrent(int16_t *pCurrent_10ma);

#endif//__PWR_MONITORING_H