#ifndef __ADC_CONTROLLER_CFG_H
#define __ADC_CONTROLLER_CFG_H

#include "hal/adc_types.h"

/******************************************************************************
*   Public Definitions
*******************************************************************************/
#define ADC_CTRL_MAX_SAMPLING_FREQ_HZ          (SOC_ADC_SAMPLE_FREQ_THRES_HIGH) 

/******************************************************************************
*   Public Macros
*******************************************************************************/


/******************************************************************************
*   Public Data Types
*******************************************************************************/
typedef enum ADC_Ctrl_Atten_e{
    ADC_CTRL_ATTEN_0DB = ADC_ATTEN_DB_0,
    ADC_CTRL_ATTEN_2_5DB = ADC_ATTEN_DB_2_5,
    ADC_CTRL_ATTEN_6DB = ADC_ATTEN_DB_6,
    ADC_CTRL_ATTEN_12DB = ADC_ATTEN_DB_12,

    ADC_CTRL_ATTEN_INVALID = ADC_ATTEN_DB_12 + 1,
}ADC_Ctrl_Atten_t;

typedef enum ADC_Ctrl_Channel_e{
    ADC_CTRL_CHANNEL_BUS_VOLT = ADC_CHANNEL_0,
    ADC_CTRL_CHANNEL_V_REF = ADC_CHANNEL_1,
    ADC_CTRL_CHANNEL_I_pB = ADC_CHANNEL_2,
    ADC_CTRL_CHANNEL_I_pA = ADC_CHANNEL_3,
    ADC_CTRL_CHANNEL_TEMP_pA = ADC_CHANNEL_4,
    ADC_CTRL_CHANNEL_TEMP_pB = ADC_CHANNEL_5,
    ADC_CTRL_CHANNEL_TEMP_LOAD = ADC_CHANNEL_6,

    ADC_CTRL_CHANNEL_INVALID = ADC_CTRL_CHANNEL_TEMP_LOAD + 1,
}ADC_Ctrl_Channel_t;

typedef uint8_t ADC_Ctrl_Channel_Mask_t;
#define ADC_CTRL_CHANNEL_BUS_VOLT_MASK          (1<<ADC_CTRL_CHANNEL_BUS_VOLT)
#define ADC_CTRL_CHANNEL_V_REF_MASK             (1<<ADC_CTRL_CHANNEL_V_REF)
#define ADC_CTRL_CHANNEL_I_pB_MASK              (1<<ADC_CTRL_CHANNEL_I_pB)
#define ADC_CTRL_CHANNEL_I_pA_MASK              (1<<ADC_CTRL_CHANNEL_I_pA)
#define ADC_CTRL_CHANNEL_TEMP_pA_MASK           (1<<ADC_CTRL_CHANNEL_TEMP_pA)
#define ADC_CTRL_CHANNEL_TEMP_pB_MASK           (1<<ADC_CTRL_CHANNEL_TEMP_pB)
#define ADC_CTRL_CHANNEL_TEMP_LOAD_MASK         (1<<ADC_CTRL_CHANNEL_TEMP_LOAD)

/******************************************************************************
*   Public Variables
*******************************************************************************/


/******************************************************************************
*   Error Check
*******************************************************************************/


/******************************************************************************
*   Public Functions Declarations
*******************************************************************************/


#endif//__ADC_CONTROLLER_CFG_H