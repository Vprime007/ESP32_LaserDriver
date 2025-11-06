#include <stdio.h>
#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_continuous.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

#include "soc/soc_caps.h"

#include "adcController.h"

/******************************************************************************
*   Private Definitions
*******************************************************************************/
#define ADC_UNIT                        (ADC_UNIT_1)
#define ADC_INVALID_CHANNEL             (0xFF)

#define LOG_LOCAL_LEVEL                 (ESP_LOG_INFO)

/******************************************************************************
*   Private Macros
*******************************************************************************/


/******************************************************************************
*   Private Data Types
*******************************************************************************/


/******************************************************************************
*   Private Functions Declaration
*******************************************************************************/
static volatile bool IRAM_ATTR continuous_conv_done_callback(adc_continuous_handle_t handle,
                                                             const adc_continuous_evt_data_t *data,
                                                             void *user_data);

/******************************************************************************
*   Public Variables
*******************************************************************************/


/******************************************************************************
*   Private Variables
*******************************************************************************/
static adc_oneshot_unit_handle_t oneshot_handle = NULL;
static adc_continuous_handle_t continuous_handle = NULL;

static uint32_t active_ctrl_channels = 0;
static volatile adcContinuousCallback_t active_continuous_callback = NULL;

static SemaphoreHandle_t adc_mutex_handle = NULL;

/******************************************************************************
*   Error Check
*******************************************************************************/


/******************************************************************************
*   Private Functions Definitions
*******************************************************************************/
static volatile bool IRAM_ATTR continuous_conv_done_callback(adc_continuous_handle_t handle,
                                                             const adc_continuous_evt_data_t *data,
                                                             void *user_data){

    BaseType_t mustYield = pdFALSE;

    //If continuous callback available -> Call it to signal sampling completion
    if(active_continuous_callback != NULL)  active_continuous_callback(data->conv_frame_buffer, data->size);

    return (mustYield == pdTRUE);
}

/******************************************************************************
*   CallBack Functions implementation
*******************************************************************************/


/******************************************************************************
*   Public Functions Definitions
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
ADC_Ctrl_Ret_t ADC_InitController(void){

    oneshot_handle = NULL;
    continuous_handle = NULL;
    active_ctrl_channels = 0;

    //Create mutex
    adc_mutex_handle = xSemaphoreCreateMutex();
    if(adc_mutex_handle == NULL){
        return ADC_CTRL_STATUS_FAIL;
    }

    return ADC_CTRL_STATUS_SUCCESS;
}

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
                                             ADC_Ctrl_Atten_t ctrl_atten){

    xSemaphoreTake(adc_mutex_handle, portMAX_DELAY);

    //check if adc is available
    if((ctrl_channel != ADC_CTRL_CHANNEL_INVALID) && 
       (active_ctrl_channels == 0)){
        //ADC ctrl available
        //Update active channel
        active_ctrl_channels |= (1ULL<<ctrl_channel);
    }
    else{   
        //ADC already in used
        //Check if the channel correspond to the active channel
        if(active_ctrl_channels != (1UL<<ctrl_channel)){
            //channel do not correspond to the active one
            xSemaphoreGive(adc_mutex_handle);
            return ADC_CTRL_STATUS_FAIL;
        }
    }

    //Init unit
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT,
    };
    adc_oneshot_new_unit(&init_config, &oneshot_handle);

    //Init channel
    adc_oneshot_chan_cfg_t chan_config = {
        .bitwidth = ADC_BITWIDTH_12,
        .atten = ctrl_atten,
    };
    adc_oneshot_config_channel(oneshot_handle, ctrl_channel, &chan_config);

    xSemaphoreGive(adc_mutex_handle);

    return ADC_CTRL_STATUS_SUCCESS;
}

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
                                                       uint16_t *pResult){

    if((ctrl_channel == ADC_CTRL_CHANNEL_INVALID) || (active_ctrl_channels != (1ULL<<ctrl_channel))){
        //Invalid channel and/or ADC not available
        return ADC_CTRL_STATUS_FAIL;
    }

    uint32_t adc_value = 0;
    for(uint32_t i=0; i<nb_samples; i++){
        int tmp_adc = 0;
        adc_oneshot_read(oneshot_handle, ctrl_channel, &tmp_adc);
        adc_value += tmp_adc;
    }
    adc_value /= nb_samples;

    if(pResult != NULL)     *pResult = adc_value;

    return ADC_CTRL_STATUS_SUCCESS;
}

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
ADC_Ctrl_Ret_t ADC_StartSampling(ADC_Ctrl_Mode_t mode, void *pConfig){

    xSemaphoreTake(adc_mutex_handle, portMAX_DELAY);

    if((mode >= ADC_CTRL_MODE_INVALID) || (pConfig == NULL)){
        xSemaphoreGive(adc_mutex_handle);
        return ADC_CTRL_STATUS_FAIL;
    }

    switch(mode){

        case ADC_CTRL_MODE_ONE_SHOT:
        {   
            //Check if ADC channel is valid
            if(((ADC_Ctrl_OneShotConfig_t*)pConfig)->ctrl_channel >= ADC_CTRL_CHANNEL_INVALID){
                xSemaphoreGive(adc_mutex_handle);
                return ADC_CTRL_STATUS_FAIL;
            }

            //check if adc is available
            if(active_ctrl_channels == 0){
                //ADC ctrl available
                //Update active channel
                active_ctrl_channels |= (1ULL<<((ADC_Ctrl_OneShotConfig_t*)pConfig)->ctrl_channel);
            }
            else{
                //ADC already in used
                //Check if the channel correspond to the active channel
                if(active_ctrl_channels != (1ULL<<((ADC_Ctrl_OneShotConfig_t*)pConfig)->ctrl_channel)){
                    //channel do not correspond to the active one
                    xSemaphoreGive(adc_mutex_handle);
                    return ADC_CTRL_STATUS_FAIL;
                }
            }

            //Check if result buffer is valid
            if(((ADC_Ctrl_OneShotConfig_t*)pConfig)->pResult == NULL){

                active_ctrl_channels = 0;
                xSemaphoreGive(adc_mutex_handle);
                return ADC_CTRL_STATUS_FAIL;
            }

            adc_channel_t channel = ((ADC_Ctrl_OneShotConfig_t*)pConfig)->ctrl_channel;

            //Init unit
            adc_oneshot_unit_init_cfg_t init_config = {
                .unit_id = ADC_UNIT,
                .ulp_mode = ADC_ULP_MODE_DISABLE,
            };
            if(ESP_OK != adc_oneshot_new_unit(&init_config, &oneshot_handle)){

                active_ctrl_channels = 0;
                xSemaphoreGive(adc_mutex_handle);
                return ADC_CTRL_STATUS_FAIL;
            }

            //Init channel
            adc_oneshot_chan_cfg_t chan_config = {
                .bitwidth = ADC_BITWIDTH_12,
                .atten = ((ADC_Ctrl_OneShotConfig_t*)pConfig)->ctrl_atten,
            };
            if(ESP_OK != adc_oneshot_config_channel(oneshot_handle, channel, &chan_config)){

                active_ctrl_channels = 0;
                adc_oneshot_del_unit(oneshot_handle);
                xSemaphoreGive(adc_mutex_handle);
                return ADC_CTRL_STATUS_FAIL;
            }

            uint32_t result = 0;
            for(uint32_t index=0; index <((ADC_Ctrl_OneShotConfig_t*)pConfig)->nb_sample; index++){

                int tmp_raw = 0;
                if(ESP_OK != adc_oneshot_read(oneshot_handle, channel, &tmp_raw)){
                    break;
                }
                result += tmp_raw;
            }
            result /= ((ADC_Ctrl_OneShotConfig_t*)pConfig)->nb_sample;

            *((ADC_Ctrl_OneShotConfig_t*)pConfig)->pResult = (uint16_t)result;

            //De-init and release ADC
            if(ESP_OK != adc_oneshot_del_unit(oneshot_handle)){
                active_ctrl_channels = 0;
                xSemaphoreGive(adc_mutex_handle);
                return ADC_CTRL_STATUS_FAIL;
            }
            oneshot_handle = NULL;//Reset the handle 
            active_ctrl_channels = 0;
        }
        break;

        case ADC_CTRL_MODE_CONTINUOUS:
        {   
            //Check if channel mask contain valid channels
            if(((ADC_Ctrl_ContinuousConfig_t*)pConfig)->channel_mask == 0){
                xSemaphoreGive(adc_mutex_handle);
                return ADC_CTRL_STATUS_FAIL;
            }

            //Check if nb channel is valid
            uint8_t nb_channel_mask = 0;
            for(uint8_t i=0; i<8; i++){
                if(((ADC_Ctrl_ContinuousConfig_t*)pConfig)->channel_mask & (1<<i)){
                    nb_channel_mask++;
                }
            }
            if(nb_channel_mask == 0){
                xSemaphoreGive(adc_mutex_handle);
                return ADC_CTRL_STATUS_FAIL;
            }
            
            //Check if number of sample is valid
            if(((ADC_Ctrl_ContinuousConfig_t*)pConfig)->nb_sample <= 0){
                xSemaphoreGive(adc_mutex_handle);        
                return ADC_CTRL_STATUS_FAIL;
            }

            //Check if callback is valid
            if(((ADC_Ctrl_ContinuousConfig_t*)pConfig)->callback == NULL){
                xSemaphoreGive(adc_mutex_handle);
                return ADC_CTRL_STATUS_FAIL;
            }

            //Check if sampling frequency is valid
            if((((ADC_Ctrl_ContinuousConfig_t*)pConfig)->sample_freq_hz <= 0) || 
               (((ADC_Ctrl_ContinuousConfig_t*)pConfig)->sample_freq_hz > ADC_CTRL_MAX_SAMPLING_FREQ_HZ)){
                
                xSemaphoreGive(adc_mutex_handle);
                return ADC_CTRL_STATUS_FAIL;
            }

            //Check if ADC is available
            if(active_ctrl_channels == 0){
                active_ctrl_channels = ((ADC_Ctrl_ContinuousConfig_t*)pConfig)->channel_mask;
            }
            else{
                if(active_ctrl_channels != ((ADC_Ctrl_ContinuousConfig_t*)pConfig)->channel_mask){
                    xSemaphoreGive(adc_mutex_handle);
                    return ADC_CTRL_STATUS_FAIL;
                }
            }

            //Init adc unit
            adc_continuous_handle_cfg_t adc_config = {
                .max_store_buf_size = ((ADC_Ctrl_ContinuousConfig_t*)pConfig)->nb_sample * SOC_ADC_DIGI_DATA_BYTES_PER_CONV * nb_channel_mask,
                .conv_frame_size = ((ADC_Ctrl_ContinuousConfig_t*)pConfig)->nb_sample * SOC_ADC_DIGI_DATA_BYTES_PER_CONV * nb_channel_mask,
            };
            if(ESP_OK != adc_continuous_new_handle(&adc_config, &continuous_handle)){
                active_ctrl_channels = 0;
                xSemaphoreGive(adc_mutex_handle);
                return ADC_CTRL_STATUS_FAIL;
            }

            //Config sampling patterns
            adc_digi_pattern_config_t pattern_config[nb_channel_mask];
            uint8_t j = 0;
            for(uint8_t i=0; i<8;i++){
                if(((((ADC_Ctrl_ContinuousConfig_t*)pConfig)->channel_mask) & (1<<i)) == (1<<i)){
                    pattern_config[j].atten = ((ADC_Ctrl_ContinuousConfig_t*)pConfig)->ctrl_atten;
                    pattern_config[j].bit_width = ADC_BITWIDTH_12;
                    pattern_config[j].channel = i;
                    pattern_config[j].unit = ADC_UNIT;
                    j++;
                }
            }

            //Config ADC for continuous sampling
            adc_continuous_config_t dig_config = {
                .sample_freq_hz = ((ADC_Ctrl_ContinuousConfig_t*)pConfig)->sample_freq_hz,
                .conv_mode = ADC_CONV_SINGLE_UNIT_1,
                .format = ADC_DIGI_OUTPUT_FORMAT_TYPE2,
                .adc_pattern = pattern_config,
                .pattern_num = nb_channel_mask,
            };
            if(ESP_OK != adc_continuous_config(continuous_handle, &dig_config)){
                active_ctrl_channels = 0;
                xSemaphoreGive(adc_mutex_handle);
                return ADC_CTRL_STATUS_FAIL;
            }
            
            //register continuous sampling callback
            active_continuous_callback = ((ADC_Ctrl_ContinuousConfig_t*)pConfig)->callback;
            adc_continuous_evt_cbs_t cbs = {
                .on_pool_ovf = continuous_conv_done_callback,
            };
            if(ESP_OK != adc_continuous_register_event_callbacks(continuous_handle, &cbs, NULL)){
                active_ctrl_channels = 0;
                xSemaphoreGive(adc_mutex_handle);
                return ADC_CTRL_STATUS_FAIL;
            }
            
            //Start continuous sampling
            if(ESP_OK != adc_continuous_start(continuous_handle)){
                active_ctrl_channels = 0;
                xSemaphoreGive(adc_mutex_handle);
                return ADC_CTRL_STATUS_FAIL;
            }
        }
        break;

        default:
        {
            xSemaphoreGive(adc_mutex_handle);
            return ADC_CTRL_STATUS_FAIL;
        }
        break;
    }

    xSemaphoreGive(adc_mutex_handle);

    return ADC_CTRL_STATUS_SUCCESS;
}

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
                                    uint32_t size){

    xSemaphoreTake(adc_mutex_handle, portMAX_DELAY);

    if(ctrl_channel >= ADC_CTRL_CHANNEL_INVALID){
        //Invalid channel
        xSemaphoreGive(adc_mutex_handle);
        return ADC_CTRL_STATUS_FAIL;
    }

    if(pResult == NULL){
        //Invalid data set
        xSemaphoreGive(adc_mutex_handle);
        return ADC_CTRL_STATUS_FAIL;
    }

    adc_cali_handle_t calib_handle = NULL;
    //Init calib
    adc_cali_curve_fitting_config_t curve_config = {
        .unit_id = ADC_UNIT,
        .chan = ctrl_channel,
        .atten = ctrl_atten,
        .bitwidth = ADC_BITWIDTH_12,
    };
    adc_cali_create_scheme_curve_fitting(&curve_config, &calib_handle);

    for(uint32_t index=0; index<size; index++){
        int raw_value = pResult[index];
        int result = 0;

        adc_cali_raw_to_voltage(calib_handle, raw_value, &result);
        pResult[index] = result;
    }

    //Delete calib scheme
    adc_cali_delete_scheme_curve_fitting(calib_handle);

    xSemaphoreGive(adc_mutex_handle);

    return ADC_CTRL_STATUS_SUCCESS;                                    
}

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
*   \param[in]  ctrl_channel        active controller channel
*
*   \return     Operation status
*
*******************************************************************************/
ADC_Ctrl_Ret_t ADC_ReleaseAdcController(ADC_Ctrl_Channel_Mask_t channel_mask){

    xSemaphoreTake(adc_mutex_handle, portMAX_DELAY);

    if(channel_mask == 0){
        xSemaphoreGive(adc_mutex_handle);
        return ADC_CTRL_STATUS_FAIL;
    }

    if(active_ctrl_channels != channel_mask){
        xSemaphoreGive(adc_mutex_handle);
        return ADC_CTRL_STATUS_FAIL;
    }

    active_ctrl_channels = 0;
    active_continuous_callback = NULL;

    //De-init continuous adc
    adc_continuous_stop(continuous_handle);
    adc_continuous_deinit(continuous_handle); 

    xSemaphoreGive(adc_mutex_handle);

    return ADC_CTRL_STATUS_SUCCESS;
}

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
ADC_Ctrl_Ret_t ADC_ReleaseAdcFromCriticalSampling(ADC_Ctrl_Channel_t ctrl_channel){

    xSemaphoreTake(adc_mutex_handle, portMAX_DELAY);

    if(ctrl_channel >= ADC_CTRL_CHANNEL_INVALID){
        //Invalid channel
        xSemaphoreGive(adc_mutex_handle);
        return ADC_CTRL_STATUS_FAIL;
    }

    if(active_ctrl_channels != (1ULL<<ctrl_channel)){
        //Channel do not correspond to the active one
        xSemaphoreGive(adc_mutex_handle);
        return ADC_CTRL_STATUS_FAIL;
    }

    //De-init and release ADC
    adc_oneshot_del_unit(oneshot_handle);
    active_ctrl_channels = 0;

    xSemaphoreGive(adc_mutex_handle);

    return ADC_CTRL_STATUS_SUCCESS;
}

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
bool ADC_IsControllerAvailable(void){

    ADC_Ctrl_Channel_t ctrl_channels = 0;

    xSemaphoreTake(adc_mutex_handle, portMAX_DELAY);
    ctrl_channels = active_ctrl_channels;
    xSemaphoreGive(adc_mutex_handle);

    return (ctrl_channels == 0);
}

/******************************************************************************
*   Interrupts
*******************************************************************************/

