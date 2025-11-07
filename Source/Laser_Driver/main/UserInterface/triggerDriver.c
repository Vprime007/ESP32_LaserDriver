#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "esp_log.h"

#include "taskPriority.h"
#include "triggerDriver.h"

/******************************************************************************
*   Private Definitions
*******************************************************************************/
#define TRIGGER_MONITORING_PERIOD_MS    (20)
#define TRIGGER_DEBOUNCE_FACTOR         (3)

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
static void tTriggerTask(void *pvParameters);

/******************************************************************************
*   Public Variables
*******************************************************************************/


/******************************************************************************
*   Private Variables
*******************************************************************************/
static TRIGGER_Config_t trigger_config = {0}; 
static triggerStateChangeCallback_t state_change_callback = NULL;
static TRIGGER_State_t current_state = TRIGGER_STATE_INVALID;
static uint32_t trigger_press_cptr = 0;
static uint32_t trigger_release_cptr = 0;

static TaskHandle_t trigger_task_handle = NULL;

static const char * TAG = "TRIGGER";

/******************************************************************************
*   Error Check
*******************************************************************************/


/******************************************************************************
*   Private Functions Definitions
*******************************************************************************/
/***************************************************************************//*!
*  \brief Trigger Task
*
*   This function is teh trigger driver task. It monitor the trigger input
*   and detect state changes (PRESS/RELEASE).
*   
*   Preconditions: None.
*
*   Side Effects: None.
*
*   \return     Operation status
*
*******************************************************************************/
static void tTriggerTask(void *pvParameters){

    ESP_LOGI(TAG, "Starting Trigger task");

    for(;;){

        //Get trigger input state
        TRIGGER_State_t new_state = TRIGGER_STATE_INVALID;
        uint8_t level = gpio_get_level(trigger_config.trigger_gpio);
        if((level && (trigger_config.active_level == TRIGGER_ACTIVE_LEVEL_HIGH)) || 
           (!level && (trigger_config.active_level == TRIGGER_ACTIVE_LEVEL_LOW))){

            new_state = TRIGGER_STATE_PRESS;
        }
        else{
            new_state = TRIGGER_STATE_RELEASE;
        }

        if(new_state == TRIGGER_STATE_PRESS){
            
            if(trigger_press_cptr < 0xFFFFFFFF)     trigger_press_cptr++;
            trigger_release_cptr = 0;

            if((trigger_press_cptr >= TRIGGER_DEBOUNCE_FACTOR) && 
               (trigger_press_cptr != 0xFFFFFFFF)){

                if(new_state != current_state){
                    trigger_press_cptr = 0xFFFFFFFF;
                    current_state = new_state;
                    if(state_change_callback != NULL)   state_change_callback(current_state);
                }
            }
        }
        else{

            if(trigger_release < 0xFFFFFFFF)    trigger_release_cptr++;
            trigger_press_cptr = 0;

            if((trigger_release_cptr >= TRIGGER_DEBOUNCE_FACTOR) && 
               (trigger_release_cptr != 0xFFFFFFFF)){

                if(new_state != current_state){
                    trigger_release_cptr = 0xFFFFFFFF;
                    current_state = new_state;
                    if(state_change_callback != NULL)   state_change_callback(current_state);
                }

            }
        }

        vTaskDelay(TRIGGER_MONITORING_PERIOD_MS/portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

/******************************************************************************
*   CallBack Functions implementation
*******************************************************************************/


/******************************************************************************
*   Public Functions Definitions
*******************************************************************************/
/***************************************************************************//*!
*  \brief Trigger Driver initialization.
*
*   This function is used to initialize the trigger driver. When a trigger
*   state change is detected, the callback function will be called with the 
*   new state as parameter.
*   
*   Preconditions: None.
*
*   Side Effects: None.
*
*   \return     Operation status
*
*******************************************************************************/
TRIGGER_Ret_t TRIGGER_InitDriver(TRIGGER_Config_t *pConfig,
                                 triggerStateChangeCallback_t callback){

    //Config gpio 
    gpio_config_t cfg = {
        .mode = GPIO_MODE_INPUT,
        .intr_type = GPIO_INTR_DISABLE,
        .pull_down_en = ((pConfig->active_level == TRIGGER_ACTIVE_LEVEL_HIGH) ? GPIO_PULLDOWN_ENABLE : GPIO_PULLDOWN_DISABLE),
        .pull_up_en = ((pConfig->active_level == TRIGGER_ACTIVE_LEVEL_LOW) ? GPIO_PULLUP_ENABLE : GPIO_PULLUP_DISABLE),
        .pin_bit_mask = (1ULL << pConfig->trigger_gpio),
    };
    if(ESP_OK != gpio_config(&cfg)){
        ESP_LOGE(TAG, "Failed to init Trigger gpio");
        return TRIGGER_STATUS_ERROR;
    }

    //Save config
    memcpy(&trigger_config, pConfig, sizeof(trigger_config));

    //register callback
    state_change_callback = callback;

    //Init current state to release
    current_state = TRIGGER_STATE_RELEASE;

    //Create Trigger task
    if(pdPASS != xTaskCreate(tTriggerTask,
                             "Trig task",
                             2048,
                             NULL,
                             TRIGGER_TASK_PRIORITY,
                             &trigger_task_handle)){

        ESP_LOGE(TAG, "Failed to create Trigger task");
        return TRIGGER_STATUS_ERROR;
    }

    return TRIGGER_STATUS_OK;
}

/******************************************************************************
*   Interrupts
*******************************************************************************/

