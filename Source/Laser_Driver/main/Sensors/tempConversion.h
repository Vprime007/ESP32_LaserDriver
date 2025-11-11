#ifndef __TEMP_CONVERSION_H
#define __TEMP_CONVERSION_H

#include <stdint.h>

/******************************************************************************
*   Public Definitions
*******************************************************************************/
#define TEMP_CONV_ERROR_INVALID         (0x8000)
#define TEMP_CONV_ERROR_OPEN            (0xFFFE)
#define TEMP_CONV_ERROR_SHORT           (0xFFFD)

/******************************************************************************
*   Public Macros
*******************************************************************************/
typedef struct TEMP_Conv_s{
    uint16_t adc_value;
    int16_t temperature;
}TEMP_Conv_t;

typedef struct TEMP_Conv_Config_s{
    TEMP_Conv_t *pConvTable;
    uint32_t table_size;
    uint16_t adc_tolerance;
}TEMP_Conv_Config_t;

/******************************************************************************
*   Public Data Types
*******************************************************************************/


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
*  \brief Convert to temperature.
*
*   This function is used to convert an adc value to it's corresponding
*   temperature value (20*C -> 2000).
*   
*   Preconditions: None.
*
*   Side Effects: None.
*
*   \param[in]  adc_value               Adc value.
*
*   \return     temperature (in 10m*c)
*
*******************************************************************************/
int16_t TCONV_ConvertToTemp(uint16_t adc_value, 
                            TEMP_Conv_Config_t *pConfig);

#endif//__TEMP_CONVERSION_H