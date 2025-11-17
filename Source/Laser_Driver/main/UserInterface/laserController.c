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
static LED_Handle_t first_laser_handle;
static LED_Handle_t second_laser_handle;

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
    if(LDRV_STATUS_OK != LDRV_AddLedSinglePwm(laser_cfg, &first_laser_handle)){
        ESP_LOGE(TAG, "Failed to register the first laser diode");
        return LASER_STATUS_ERROR;
    }

    laser_cfg.gpio_num = HWI_PB_DIM_GPIO;
    laser_cfg.led_channel = LEDC_CHANNEL_1;
    if(LDRV_STATUS_OK != LDRV_AddLedSinglePwm(laser_cfg, &second_laser_handle)){
        ESP_LOGE(TAG, "Failed to register the second laser diode");
        return LASER_STATUS_ERROR;
    }

    LDRV_SetLedSinglePwmDuty(LDRV_CFG_MIN_PWM_DUTY, first_laser_handle);
    LDRV_SetLedSinglePwmDuty(LDRV_CFG_MIN_PWM_DUTY, second_laser_handle);

    return LASER_STATUS_OK;
}

LASER_Ret_t LASER_SetActive(uint16_t percent){

    if(laser_mutex_handle == NULL){
        ESP_LOGE(TAG, "Laser controller not initialized");
        return LASER_STATUS_ERROR;
    }

    if((percent >= LASER_MAX_PERCENT) || (percent <= LASER_MIN_PERCENT)){
        ESP_LOGE(TAG, "Invalid params");
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
        xSemaphoreGive(laser_mutex_handle);
    }

    return LASER_STATUS_OK;
}

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
        xSemaphoreGive(laser_mutex_handle);
    }

    return LASER_STATUS_OK;
}

LASER_Ret_t LASER_GetState(LASER_State_t *pState){

    if(pState == NULL){
        ESP_LOGI(TAG, "Invalid params");
        return LASER_STATUS_ERROR;
    }

    xSemaphoreTake(laser_mutex_handle, portMAX_DELAY);
    *pState = laser_state;
    xSemaphoreGive(laser_mutex_handle);

    return LASER_STATUS_OK;
}


/******************************************************************************
*   Interrupts
*******************************************************************************/


