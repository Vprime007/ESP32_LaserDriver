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
PWR_Ret_t PWR_InitMonitoring(void);

PWR_Ret_t PWR_ProcessRawMeasurement(uint8_t *pMeas_buf, uint32_t size);

PWR_Ret_t PWR_GetBusVoltage(int16_t *pVoltage_10mv);

PWR_Ret_t PWR_GetPhaseACurrent(int16_t *pCurrent_mv);

PWR_Ret_t PWR_GetPhaseBCurrent(int16_t *pCurrent_mv);

#endif//__PWR_MONITORING_H