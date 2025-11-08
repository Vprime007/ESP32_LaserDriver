#ifndef __TEMPERATURE_MONITORING_H
#define __TEMPERATURE_MONITORING_H

#include <stdint.h>

/******************************************************************************
*   Public Definitions
*******************************************************************************/
#define TEMP_ERROR_INVALID              (0x8000)
#define TEMP_ERROR_SHORT                (0xFFFE)
#define TEMP_ERROR_OPEN                 (0xFFFD)

/******************************************************************************
*   Public Macros
*******************************************************************************/


/******************************************************************************
*   Public Data Types
*******************************************************************************/
typedef enum TEMP_Sensor_Id_e{
    TEMP_SENSOR_ID_PHASE_A,
    TEMP_SENSOR_ID_PHASE_B,
    TEMP_SENSOR_ID_LOAD,

    TEMP_SENSOR_ID_INVALID,
}TEMP_Sensor_Id_t;

typedef enum TEMP_Ret_e{
    TEMP_STATUS_ERROR,
    TEMP_STATUS_OK,
}TEMP_Ret_t;

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
*  \brief Temperature Monitoring initialization.
*
*   This function is used to initialize the temperature monitoring
*   module.
*   
*   Preconditions: None.
*
*   Side Effects: None.
*
*   \return     Operation status
*
*******************************************************************************/
TEMP_Ret_t TEMP_InitMonitoring(void);

/***************************************************************************//*!
*  \brief Process Raw temperature measurements.
*
*   This function is used to process raw temperature measurement from the 
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
TEMP_Ret_t TEMP_ProcessRawMeasurement(uint8_t *pMeas_buf, uint32_t size);

/***************************************************************************//*!
*  \brief Get temperature.
*
*   This function is used get the latest temperature value of a temperature 
*   sensor.
*   
*   Preconditions: None.
*
*   Side Effects: None.
*
*   \param[in]  sensor_id           Sensor ID.
*   \param[in]  pTemperature        Pointer to store temperature value.  
*
*   \return     Operation status
*
*******************************************************************************/
TEMP_Ret_t TEMP_GetTemperature(TEMP_Sensor_Id_t sensor_id, int16_t *pTemperature);


#endif//__TEMPERATURE_MONITORING_H