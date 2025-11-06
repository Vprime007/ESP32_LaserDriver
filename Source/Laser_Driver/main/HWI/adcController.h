#ifndef __ADC_CONTROLLER_H
#define __ADC_CONTROLLER_H

#include <stdint.h>
#include <stdbool.h>

#include "adcController_cfg.h"

/******************************************************************************
*   Public Definitions
*******************************************************************************/
#define ADC_CONTINUOUS_SAMPLE_SIZE_BYTE             (4)

/******************************************************************************
*   Public Macros
*******************************************************************************/
typedef void(*adcContinuousCallback_t)(uint8_t *pResults, uint32_t frame_size);

/******************************************************************************
*   Public Data Types
*******************************************************************************/
typedef enum ADC_Ctrl_Mode_e{
    ADC_CTRL_MODE_ONE_SHOT,
    ADC_CTRL_MODE_CONTINUOUS,

    ADC_CTRL_MODE_INVALID,
}ADC_Ctrl_Mode_t;

typedef struct ADC_Ctrl_OneShotConfig_s{
    const ADC_Ctrl_Channel_t ctrl_channel;
    const ADC_Ctrl_Atten_t ctrl_atten;
    const uint32_t nb_sample;
    uint16_t *pResult;
}ADC_Ctrl_OneShotConfig_t;

typedef struct ADC_Ctrl_ContinuousConfig_s{
    const ADC_Ctrl_Channel_Mask_t channel_mask;
    const ADC_Ctrl_Atten_t ctrl_atten;
    const uint32_t nb_sample;
    const uint32_t sample_freq_hz;
    adcContinuousCallback_t callback;
}ADC_Ctrl_ContinuousConfig_t;

typedef enum ADC_Ctrl_Ret_e{
    ADC_CTRL_STATUS_FAIL,
    ADC_CTRL_STATUS_SUCCESS,
}ADC_Ctrl_Ret_t;

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
*  \brief ADC controller initialization.
*
*   This function perform the ADC Controller module initialization.
*   
*   Preconditions: None.
*
*   Side Effects: None.
*
*   \return     Operation status
*
*******************************************************************************/
ADC_Ctrl_Ret_t ADC_InitController(void);

/***************************************************************************//*!
*  \brief Setup Time Critical sampling
*
*   This function is used to configure the ADC for a time critical sampling 
*   (sampling inside an interrupt routine). Can only use OneShot mode for 
*   time critical sampling.
*   
*   Preconditions: None.
*
*   Side Effects: None.
*
*   \param[in]  ctrl_channel        ADC ctrl channel to sample.
*   \param[in]  ctrl_atten          ADC ctrl attenuation.
*
*   \return     Operation status
*
*******************************************************************************/
ADC_Ctrl_Ret_t ADC_SetupTimeCriticalSampling(ADC_Ctrl_Channel_t ctrl_channel, 
                                             ADC_Ctrl_Atten_t ctrl_atten);

/***************************************************************************//*!
*  \brief Start Time Critical sampling
*
*   This function is used to start the ADC for time critical measurements.
*   The ADC must have been config for time critical sampling by calling
*   the ADC_SetuptTimeCriticalSampling() function previously.
*   
*   Preconditions: ADC Ctrl setup for time critical sampling.
*
*   Side Effects: None.
*
*   \param[in]      ctrl_channel        ADC ctrl channel to sample.
*   \param[in]      nb_samples          Number of consecutive sample to take.
*   \param[out]     pResult             Pointer to store the ADC result.
*
*   \return     Operation status
*
*******************************************************************************/
ADC_Ctrl_Ret_t IRAM_ATTR ADC_StartTimeCriticalSampling(ADC_Ctrl_Channel_t ctrl_channel,
                                                       uint32_t nb_samples,
                                                       uint16_t *pResult);

/***************************************************************************//*!
*  \brief Start ADC sampling
*
*   This function is used to start ADC sampling.
*   
*   Preconditions: None.
*
*   Side Effects: None.
*
*   \param[in]  mode                    ADC controller mode
*   \param[in]  pConfig                 pointer to ADC ctrl config  
*
*   \return     Operation status
*
*******************************************************************************/
ADC_Ctrl_Ret_t ADC_StartSampling(ADC_Ctrl_Mode_t mode, void *pConfig);

/***************************************************************************//*!
*  \brief Apply calibration.
*
*   This function is used apply calibration on raw adc value data set.
*   
*   Preconditions: None.
*
*   Side Effects: None.
*
*   \param[in]  ctrl_channel            Ctrl channel data source
*   \param[in]  ctrl_atten              Ctrl channel attenuation
*   \param[in]  pResult                 Pointer to data set
*   \param[in]  size                    data set size   
*
*   \return     Operation status
*
*******************************************************************************/
ADC_Ctrl_Ret_t ADC_ApplyCalibration(ADC_Ctrl_Channel_t ctrl_channel, 
                                    ADC_Ctrl_Atten_t ctrl_atten,
                                    uint16_t *pResult, 
                                    uint32_t size);

/***************************************************************************//*!
*  \brief Release ADC controller
*
*   This function is used to release the ADC controller.
*   (Must be called after sampling in order to sample new channel)
*   
*   Preconditions: None.
*
*   Side Effects: None.
*
*   \param[in]  ctrl_channel_mask        active controller channel mask
*
*   \return     Operation status
*
*******************************************************************************/
ADC_Ctrl_Ret_t ADC_ReleaseAdcController(ADC_Ctrl_Channel_Mask_t channel_mask);

/***************************************************************************//*!
*  \brief Release ADC controller from critial sampling
*
*   This function is used to release the ADC controller from critical sampling.
*   (Must be called after sampling in order to sample new channel)
*   
*   Preconditions: None.
*
*   Side Effects: None.
*
*   \param[in]  ctrl_channel        active controller channel
*
*   \return     Operation status
*
*******************************************************************************/
ADC_Ctrl_Ret_t ADC_ReleaseAdcFromCriticalSampling(ADC_Ctrl_Channel_t ctrl_channel);

/***************************************************************************//*!
*  \brief Is ADC controller available
*
*   This function return if the adc in currently available.
*   
*   Preconditions: None.
*
*   Side Effects: None.
*
*   \return     ADC available (True) or unavailable (False)
*
*******************************************************************************/
bool ADC_IsControllerAvailable(void);

#endif//__ADC_CONTROLLER_H