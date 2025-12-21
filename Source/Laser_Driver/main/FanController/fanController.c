#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include "esp_log.h"
#include "driver/gpio.h"

#include "hwi.h"
#include "fanController.h"

/******************************************************************************
*   Private Definitions
*******************************************************************************/
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


/******************************************************************************
*   Public Variables
*******************************************************************************/


/******************************************************************************
*   Private Variables
*******************************************************************************/
static SemaphoreHandle_t fan_mutex = NULL;

static const char * TAG = "FAN";

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
*  \brief Fan controller initialization.
*
*   This function is used to initialize the fan controller module.
*   
*   Preconditions: None.
*
*   Side Effects: None.
*
*   \return     Operation status
*
*******************************************************************************/
FAN_Ret_t FAN_InitController(void){

    //Create mutex
    fan_mutex = xSemaphoreCreateMutex();
    if(fan_mutex == NULL){
        return FAN_STATUS_ERROR;
    }

    //Init gpios
    gpio_config_t fan_cfg = {
        .mode = GPIO_MODE_OUTPUT,
        .intr_type = GPIO_INTR_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pin_bit_mask = (1ULL << HWI_LOAD_FAN_GPIO) | 
                        (1ULL << HWI_PHASE_FAN_GPIO),
    };
    if(ESP_OK != gpio_config(&fan_cfg)){
        return FAN_STATUS_ERROR;
    }

    gpio_set_level(HWI_LOAD_FAN_GPIO, 0);//Turn load fan off
    gpio_set_level(HWI_PHASE_FAN_GPIO, 0);//Turn phase fan off

    return FAN_STATUS_OK;
}

/***************************************************************************//*!
*  \brief Set fan state.
*
*   This function is used to enable/disable a fan.
*   
*   Preconditions: None.
*
*   Side Effects: None.
*
*   \param[in]  fan_id              Target fan ID.
*   \param[in]  state               Fan state (enabel/Disable)
*
*   \return     Operation status
*
*******************************************************************************/
FAN_Ret_t FAN_SetState(FAN_Id_t fan_id, FAN_State_t state){

    if((fan_id >= FAN_ID_INVALID) || (state >= FAN_STATE_INVALID)){
        return FAN_STATUS_ERROR;
    }

    xSemaphoreTake(fan_mutex, portMAX_DELAY);

    switch(fan_id){
        case FAN_ID_LOAD:
        {
            gpio_set_level(HWI_LOAD_FAN_GPIO, (state == FAN_STATE_ENABLE) ? 1 : 0);
        }
        break;

        case FAN_ID_PHASE:
        {
            gpio_set_level(HWI_PHASE_FAN_GPIO, (state == FAN_STATE_ENABLE) ? 1 : 0);
        }
        break;

        default:
        {
            //Not supposed to be here...
        }
        break;
    }

    xSemaphoreGive(fan_mutex);

    return FAN_STATUS_OK;
}

/***************************************************************************//*!
*  \brief Get fan state.
*
*   This function is used to get the current state of a fan.
*   
*   Preconditions: None.
*
*   Side Effects: None.
*
*   \param[in]  fan_id              Target fan ID.
*   \param[in]  pState              Pointer to store fan state.
*
*   \return     Operation status
*
*******************************************************************************/
FAN_Ret_t FAN_GetState(FAN_Id_t fan_id, FAN_State_t *pState){

    if((fan_id >= FAN_ID_INVALID) || (pState == NULL)){
        return FAN_STATUS_ERROR;
    }

    xSemaphoreTake(fan_mutex, portMAX_DELAY);

    switch(fan_id){
        case FAN_ID_LOAD:
        {
            if(1 == gpio_get_level(HWI_LOAD_FAN_GPIO)){
                *pState = FAN_STATE_ENABLE;
            }
            else{
                *pState = FAN_STATE_DISABLE;
            }
        }
        break;

        case FAN_ID_PHASE:
        {
            if(1 == gpio_get_level(HWI_PHASE_FAN_GPIO)){
                *pState = FAN_STATE_ENABLE;
            }
            else{
                *pState = FAN_STATE_DISABLE;
            }
        }
        break;

        default:
        {
            //Not supposed to be here...
        }
        break;
    }

    xSemaphoreGive(fan_mutex);

    return FAN_STATUS_OK;
}

/******************************************************************************
*   Interrupts
*******************************************************************************/