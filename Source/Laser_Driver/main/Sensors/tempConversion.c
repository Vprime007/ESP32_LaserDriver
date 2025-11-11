#include <stdlib.h>

#include "tempConversion.h"

/******************************************************************************
*   Private Definitions
*******************************************************************************/


/******************************************************************************
*   Private Macros
*******************************************************************************/


/******************************************************************************
*   Private Data Types
*******************************************************************************/


/******************************************************************************
*   Private Functions Declaration
*******************************************************************************/
static int16_t adcToTemp(uint16_t adc_value, TEMP_Conv_Config_t *pConfig);
static int16_t temperatureInterpolation(uint16_t adc_value, 
                                        TEMP_Conv_t *pPrev, 
                                        TEMP_Conv_t *pNext);

/******************************************************************************
*   Public Variables
*******************************************************************************/


/******************************************************************************
*   Private Variables
*******************************************************************************/


/******************************************************************************
*   Private Functions Definitions
*******************************************************************************/
/***************************************************************************//*!
*  \brief ADC to temperature.
*
*   This function is used to convert an adc value to it's corresponding
*   temperature. If adc_value is out of range, the appropriate error
*   value will be used.
*   
*   Preconditions: None.
*
*   Side Effects: None.
*
*   \param[in]  adc_value               Adc value.
*   \param[in]  pConfig                 Pointer to conversion config.
*
*   \return     temperature (in 10m*c)
*
*******************************************************************************/
static int16_t adcToTemp(uint16_t adc_value, TEMP_Conv_Config_t *pConfig){

    if(pConfig == NULL){
        return TEMP_CONV_ERROR_INVALID;
    }

    //Check if we are below the conversion table
    if(adc_value < (pConfig->pConvTable[0].adc_value - pConfig->adc_tolerance)){
        return TEMP_CONV_ERROR_OPEN;
    }
    else if((adc_value > (pConfig->pConvTable[0].adc_value - pConfig->adc_tolerance)) && 
            (adc_value <= pConfig->pConvTable[0].adc_value)){
        
        //We are at the minimum value with tolerance -> return minimum temperature
        return pConfig->pConvTable[0].temperature;
    }
    else{
        //Do nothing... for now...
    }

    //Check if we are over the conversion table
    if(adc_value > (pConfig->pConvTable[pConfig->table_size-1].adc_value + pConfig->adc_tolerance)){
        return TEMP_CONV_ERROR_SHORT;
    }
    else if((adc_value < (pConfig->pConvTable[pConfig->table_size-1].adc_value + pConfig->adc_tolerance)) && 
            (adc_value >= pConfig->pConvTable[pConfig->table_size-1].adc_value)){
        
        //We are at the maximum value with tolerance -> return maximum temperature
        return pConfig->pConvTable[pConfig->table_size-1].temperature;
    }
    else{
        //Do nothing... for now...
    }

    //Scan conversion table
    uint32_t index = 0;
    for(index=0; index<pConfig->table_size; index++){
        if(adc_value < pConfig->pConvTable[index].adc_value)    break;
    }

    return temperatureInterpolation(adc_value, &(pConfig->pConvTable[index-1]), &(pConfig->pConvTable[index]));
}

/***************************************************************************//*!
*  \brief temperature Interpolation
*
*   This function is used to interpolate an adc value within two discrete
*   conversion table temp/adc value pair.
*
*   Preconditions: None.
*
*   Side Effects: None.
*
*   \param[in]  adc_value               Adc value.
*   \param[in]  pPrev                   Pointer to Previous temp/adc value pair
*   \param[in]  pNext                   Pointer to Next temp/adc value pair
*
*   \return     temperature (in 10m*c)
*
*******************************************************************************/
static int16_t temperatureInterpolation(uint16_t adc_value, 
                                        TEMP_Conv_t *pPrev, 
                                        TEMP_Conv_t *pNext){

    uint16_t delta_adc = pNext->adc_value - pPrev->adc_value;
    int16_t result = pPrev->temperature * (pNext->adc_value - adc_value);

    result += pNext->temperature * (adc_value - pPrev->adc_value);
    result /= delta_adc;

    return result;
}


/******************************************************************************
*   Public Functions Definitions
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
                            TEMP_Conv_Config_t *pConfig){

    int16_t temperature = TEMP_CONV_ERROR_INVALID;

    if(pConfig == NULL){
        return temperature;
    }

    //Convert adc value to temperature 
    temperature = adcToTemp(adc_value, pConfig);

    return temperature;
}

/******************************************************************************
*   Interrupts
*******************************************************************************/

