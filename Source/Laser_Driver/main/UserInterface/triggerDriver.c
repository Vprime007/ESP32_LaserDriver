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

static TaskHandle_t trigger_task_handle = NULL;

static const char * TAG = "TRIGGER";

/******************************************************************************
*   Error Check
*******************************************************************************/


/******************************************************************************
*   Private Functions Definitions
*******************************************************************************/
static void tTriggerTask(void *pvParameters){

    ESP_LOGI(TAG, "Starting Trigger task");

    for(;;){

        //Get trigger input state

        //if a transitio

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

