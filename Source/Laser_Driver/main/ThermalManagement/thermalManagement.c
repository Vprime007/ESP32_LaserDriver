
#include "esp_log.h"
#include "driver/gpio.h"

#include "hwi.h"
#include "thermalManagement.h"

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
static THERMAL_State_t load_fan_state = THERMAL_STATE_DISABLE;
static THERMAL_State_t phase_fan_state = THERMAL_STATE_DISABLE;

static const char * TAG = "Thermal";

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
*  \brief Thermal management initialization.
*
*   This function is used to initialize the thermal management module.
*   
*   Preconditions: None.
*
*   Side Effects: None.
*
*   \return     Operation status
*
*******************************************************************************/
THERMAL_Ret_t THERMAL_InitManager(void){

    //Init gpios
    gpio_config_t cfg = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pin_bit_mask = (1ULL << HWI_PHASE_FAN_GPIO) | (1ULL << HWI_LOAD_FAN_GPIO),
    };
    gpio_config(&cfg);

    //Force thermal management sources OFF
    gpio_set_level(HWI_PHASE_FAN_GPIO, 0);
    gpio_set_level(HWI_LOAD_FAN_GPIO, 0);

    return THERMAL_STATUS_OK;
}

/***************************************************************************//*!
*  \brief Set thermal management source state.
*
*   This function is used to enable/disable a thermal management source.
*   
*   Preconditions: None.
*
*   Side Effects: None.
*
*   \param[in]  source              Thermal management source.
*   \param[in]  state               Source state (enable/Disable).
*
*   \return     Operation status
*
*******************************************************************************/
THERMAL_Ret_t THERMAL_SetState(THERMAL_Source_t source, THERMAL_State_t state){

    if(source >= THERMAL_SRC_INVALID || state >= THERMAL_STATE_INVALID){
        ESP_LOGI(TAG, "Failed to set thermal source state: Invalid params");
        return THERMAL_STATUS_ERROR;
    }

    switch(source){

        case THERMAL_SRC_PHASE_FAN:
        {
            //Process only if new state differ from the previous one
            if(state != phase_fan_state){
                gpio_set_level(HWI_PHASE_FAN_GPIO, ((state == THERMAL_STATE_ENABLE) ? 1 : 0));
            }
        }
        break;

        case THERMAL_SRC_LOAD_FAN:
        {
            //Process only is the new state differ from the previous one
            if(state != load_fan_state){
                gpio_set_level(HWI_LOAD_FAN_GPIO, ((state == THERMAL_STATE_ENABLE) ? 1 : 0));
            }
        }
        break;

        default:
        {
            //Not supposed to be here...
        }
        break;
    }

    return THERMAL_STATUS_OK;
}

/***************************************************************************//*!
*  \brief Get thermal management source state.
*
*   This function is used to get the current state of a thermal 
*   management source.
*   
*   Preconditions: None.
*
*   Side Effects: None.
*
*   \param[in]  source              Thermal management source.
*   \param[in]  pState              Pointer to store source state.
*
*   \return     Operation status
*
*******************************************************************************/
THERMAL_Ret_t THERMAL_GetState(THERMAL_Source_t source, THERMAL_State_t *pState){

    if(source >= THERMAL_SRC_INVALID || pState == NULL){
        ESP_LOGI(TAG, "Failed to get source state: Invalid params");
        return THERMAL_STATUS_ERROR;
    }

    switch(source){
        case THERMAL_SRC_LOAD_FAN:
        {
            *pState = load_fan_state;
        }
        break;

        case THERMAL_SRC_PHASE_FAN:
        {
            *pState = phase_fan_state;
        }
        break;

        default:
        {
            //Not supposed to be here...
        }
        break;
    }

    return THERMAL_STATUS_OK;
}

/******************************************************************************
*   Interrupts
*******************************************************************************/


