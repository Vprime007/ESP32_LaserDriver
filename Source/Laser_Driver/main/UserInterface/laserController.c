#include <stdint.h>

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include "esp_log.h"

#include "hwi.h"
#include "ledDriver.h"
#include "laserController.h"

/******************************************************************************
*   Private Definitions
*******************************************************************************/
#define LOG_LOCAL_LEVEL             (ESP_LOG_INFO)

/******************************************************************************
*   Private Macros
*******************************************************************************/
#define PERCENT_TO_DUTY(x)          ((x*LDRV_CFG_MAX_PWM_DUTY)/LASER_MAX_PERCENT)

/******************************************************************************
*   Private Data Types
*******************************************************************************/


/******************************************************************************
*   Private Functions Declaration
*******************************************************************************/


/******************************************************************************
*   Public Variables
*******************************************************************************/


/******************************************************************************
*   Private Variables
*******************************************************************************/
static SemaphoreHandle_t laser_mutex_handle = NULL;

static LASER_State_t laser_state = LASER_STATE_INVALID;
static uint16_t active_percent = LASER_MIN_PERCENT;
static LED_Handle_t pa_laser_handle;
static LED_Handle_t pb_laser_handle;

static const char * TAG = "LASER";

/******************************************************************************
*   Error Check
*******************************************************************************/


/******************************************************************************
*   Private Functions Definitions
*******************************************************************************/


/******************************************************************************
*   CallBack Functions implementation
*******************************************************************************/


/******************************************************************************
*   Public Functions Definitions
*******************************************************************************/
/***************************************************************************//*!
*  \brief Laser Controller initialization.
*
*   This function perform the initialization of the laser controller module.
*   It also add and managed both laser instances.
*   
*   Preconditions: None.
*
*   Side Effects: None.
*
*   \return         Operation status
*
*******************************************************************************/
LASER_Ret_t LASER_InitController(void){

    //Create laser mutex
    laser_mutex_handle = xSemaphoreCreateMutex();
    if(laser_mutex_handle == NULL){
        ESP_LOGE(TAG, "Failed to create laser mutex");
    }

    //Register the 2 laser diodes as led
    LDRV_CFG_Single_Pwm_Config_t laser_cfg = {
        .gpio_num = HWI_PA_DIM_GPIO,
        .led_timer = LEDC_TIMER_0,
        .led_channel = LEDC_CHANNEL_0,
        .active_level = LDRV_CFG_ACTIVE_HIGH,
    };
    if(LDRV_STATUS_OK != LDRV_AddLedSinglePwm(laser_cfg, &pa_laser_handle)){
        ESP_LOGE(TAG, "Failed to register the first laser diode");
        return LASER_STATUS_ERROR;
    }

    laser_cfg.gpio_num = HWI_PB_DIM_GPIO;
    laser_cfg.led_channel = LEDC_CHANNEL_1;
    if(LDRV_STATUS_OK != LDRV_AddLedSinglePwm(laser_cfg, &pb_laser_handle)){
        ESP_LOGE(TAG, "Failed to register the second laser diode");
        return LASER_STATUS_ERROR;
    }

    LDRV_SetLedSinglePwmDuty(PERCENT_TO_DUTY(LDRV_CFG_MIN_PWM_DUTY), 
                             pa_laser_handle);

    LDRV_SetLedSinglePwmDuty(PERCENT_TO_DUTY(LDRV_CFG_MIN_PWM_DUTY), 
                             pb_laser_handle);

    return LASER_STATUS_OK;
}

/***************************************************************************//*!
*  \brief Set laser active percentage.
*
*   This function is used to set the Active state fade duty-cycle in percent.
*   (100% -> 10000). The laser module use a pwm to fade the laser output.
*   The percent value represent the duty-cycle of the fading pwm.
*   
*   Preconditions: None.
*
*   Side Effects: None.
*
*   \param[in]      percent             Laser pwm duty-cycle percent.
*
*   \return         Operation status
*
*******************************************************************************/
LASER_Ret_t LASER_SetActivePercent(uint16_t percent){

    if(laser_mutex_handle == NULL){
        ESP_LOGE(TAG, "Laser controller not initialized");
        return LASER_STATUS_ERROR;
    }

    if((percent >= LASER_MAX_PERCENT) || (percent <= LASER_MIN_PERCENT)){
        ESP_LOGE(TAG, "Invalid params");
        return LASER_STATUS_ERROR;
    }

    xSemaphoreTake(laser_mutex_handle, portMAX_DELAY);
    active_percent = percent;//Update active percent value
    //If laser is in active state -> update pwm duty now
    if(laser_state == LASER_STATE_ACTIVE){
        //Set new duty-cycle
        if(LDRV_STATUS_OK != LDRV_SetLedSinglePwmDuty(PERCENT_TO_DUTY(percent), 
                                                      pa_laser_handle)){
            xSemaphoreGive(laser_mutex_handle);
            return LASER_STATUS_ERROR;
        }

        if(LDRV_STATUS_OK != LDRV_SetLedSinglePwmDuty(PERCENT_TO_DUTY(percent), 
                                                      pb_laser_handle)){
            xSemaphoreGive(laser_mutex_handle);
            return LASER_STATUS_ERROR;
        }
    }
    xSemaphoreGive(laser_mutex_handle);

    return LASER_STATUS_OK;
}

/***************************************************************************//*!
*  \brief Set laser active
*
*   This function activate both laser phases and set the pwms duty-cycles to the
*   active percent value. 
*   
*   Preconditions: None.
*
*   Side Effects: None.
*
*   \return         Operation status
*
*******************************************************************************/
LASER_Ret_t LASER_SetActive(void){

    if(laser_mutex_handle == NULL){
        ESP_LOGE(TAG, "Laser controller not initialized");
        return LASER_STATUS_ERROR;
    }

    LASER_State_t current_state = LASER_STATE_INVALID;
    xSemaphoreTake(laser_mutex_handle, portMAX_DELAY);
    current_state = laser_state;
    xSemaphoreGive(laser_mutex_handle);

    //Check if laser is not already active
    if(current_state != LASER_STATE_ACTIVE){
        xSemaphoreTake(laser_mutex_handle, portMAX_DELAY);
        laser_state = LASER_STATE_ACTIVE;//Update state

        //Apply active percent duty-cycle
        if(LDRV_STATUS_OK != LDRV_SetLedSinglePwmDuty(PERCENT_TO_DUTY(active_percent), 
                                                      pa_laser_handle)){
            xSemaphoreGive(laser_mutex_handle);
            return LASER_STATUS_ERROR;
        }

        if(LDRV_STATUS_OK != LDRV_SetLedSinglePwmDuty(PERCENT_TO_DUTY(active_percent), 
                                                      pb_laser_handle)){
            xSemaphoreGive(laser_mutex_handle);
            return LASER_STATUS_ERROR;
        }

        xSemaphoreGive(laser_mutex_handle);
    }

    return LASER_STATUS_OK;
}

/***************************************************************************//*!
*  \brief Set laser inactive.
*
*   This function disable both laser phases and set the pwms duty-cycles to the
*   minimum allowed.
*   
*   Preconditions: None.
*
*   Side Effects: None.
*
*   \return         Operation status
*
*******************************************************************************/
LASER_Ret_t LASER_SetInactive(void){

    if(laser_mutex_handle == NULL){
        ESP_LOGE(TAG, "Laser controller not initialized");
        return LASER_STATUS_ERROR;
    }

    LASER_State_t current_state = LASER_STATE_INVALID;
    xSemaphoreTake(laser_mutex_handle, portMAX_DELAY);
    current_state = laser_state;
    xSemaphoreGive(laser_mutex_handle);

    //Check if laser is not already inactive
    if(current_state != LASER_STATE_INACTIVE){
        xSemaphoreTake(laser_mutex_handle, portMAX_DELAY);
        laser_state = LASER_STATE_INACTIVE;//Update state

        //Apply min percent duty-cycle
        if(LDRV_STATUS_OK != LDRV_SetLedSinglePwmDuty(PERCENT_TO_DUTY(LASER_MIN_PERCENT), 
                                                      pa_laser_handle)){
            xSemaphoreGive(laser_mutex_handle);
            return LASER_STATUS_ERROR;
        }

        if(LDRV_STATUS_OK != LDRV_SetLedSinglePwmDuty(PERCENT_TO_DUTY(LASER_MIN_PERCENT), 
                                                      pb_laser_handle)){
            xSemaphoreGive(laser_mutex_handle);
            return LASER_STATUS_ERROR;
        }
        xSemaphoreGive(laser_mutex_handle);
    }

    return LASER_STATUS_OK;
}

/***************************************************************************//*!
*  \brief Get laser state.
*
*   This function is used to get the current lasers state (Active/Inactive).
*   
*   Preconditions: None.
*
*   Side Effects: None.
*
*   \param[out]     pState                  Pointer to store state.
*
*   \return         Operation status
*
*******************************************************************************/
LASER_Ret_t LASER_GetState(LASER_State_t *pState){

    if(laser_mutex_handle == NULL){
        ESP_LOGE(TAG, "Laser controller not initialized");
        return LASER_STATUS_ERROR;
    }

    if(pState == NULL){
        ESP_LOGI(TAG, "Invalid params");
        return LASER_STATUS_ERROR;
    }

    xSemaphoreTake(laser_mutex_handle, portMAX_DELAY);
    *pState = laser_state;
    xSemaphoreGive(laser_mutex_handle);

    return LASER_STATUS_OK;
}

/***************************************************************************//*!
*  \brief Get Active persent.
*
*   This function is used to get the current active percent pwm values.
*   (100% -> 10000)
*   
*   Preconditions: None.
*
*   Side Effects: None.
*
*   \param[out]     pState                  Pointer to store the percent.
*
*   \return         Operation status
*
*******************************************************************************/
LASER_Ret_t LASER_GetActivePercent(uint16_t *pPercent){

    if(laser_mutex_handle == NULL){
        ESP_LOGE(TAG, "Laser controller not initialized");
        return LASER_STATUS_ERROR;
    }

    if(pPercent == NULL){
        ESP_LOGI(TAG, "Invalid params");
        return LASER_STATUS_ERROR;
    }

    xSemaphoreTake(laser_mutex_handle, portMAX_DELAY);
    *pPercent = active_percent;
    xSemaphoreGive(laser_mutex_handle);

    return LASER_STATUS_OK;
}


/******************************************************************************
*   Interrupts
*******************************************************************************/


