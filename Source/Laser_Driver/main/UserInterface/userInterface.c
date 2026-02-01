#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "taskPriority.h"
#include "hwi.h"
#include "triggerDriver.h"
#include "alarmController.h"
#include "laserController.h"
#include "ledController.h"
#include "userInterface.h"

/******************************************************************************
*   Private Definitions
*******************************************************************************/
#define UI_EVENT_QUEUE_SIZE             (8)
#define UI_EVENT_TIMEOUT_MS             (250)

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
static void tUiTask(void *pvParameters);

static void triggerCallback(TRIGGER_State_t state);

/******************************************************************************
*   Public Variables
*******************************************************************************/


/******************************************************************************
*   Private Variables
*******************************************************************************/
static TaskHandle_t ui_task_handle = NULL;
static QueueHandle_t ui_event_queue_handle = NULL;

static const char * TAG = "UI";

/******************************************************************************
*   Error Check
*******************************************************************************/


/******************************************************************************
*   Private Functions Definitions
*******************************************************************************/
static void triggerCallback(TRIGGER_State_t state){

    if(state == TRIGGER_STATE_PRESS){
        ESP_LOGI(TAG, "Trigger Pressed!");
        //Check if a temperature alarm is active
        ALARM_State_t phase_a_ovt = ALARM_STATE_INVALID;
        ALARM_State_t phase_b_ovt = ALARM_STATE_INVALID;
        ALARM_State_t load_ovt = ALARM_STATE_INVALID;

        ALARM_GetState(ALARM_SRC_PHASE_A_TEMP, &phase_a_ovt);
        ALARM_GetState(ALARM_SRC_PHASE_B_TEMP, &phase_b_ovt);
        ALARM_GetState(ALARM_SRC_LOAD_TEMP, &load_ovt);

        if(phase_a_ovt != ALARM_STATE_INACTIVE){
            ESP_LOGI(TAG, "Trigger blocked by phase A OVT");
            return;
        }

        if(phase_b_ovt != ALARM_STATE_INACTIVE){
            ESP_LOGI(TAG, "Trigger blocked by phase B OVT");
            return;
        }

        if(load_ovt != ALARM_STATE_INACTIVE){
            ESP_LOGI(TAG, "Trigger blocked by load OVT");
            return;
        }

        //Power-up laser diode
        if(LASER_STATUS_OK != LASER_SetAllPhaseActive()){
            ESP_LOGI(TAG, "Failed to turn laserON!!");
        }

    }
    else{
        ESP_LOGI(TAG, "Trigger Released!");

        //Power-down laser diode
        if(LASER_STATUS_OK != LASER_SetAllPhaseInactive()){
            ESP_LOGI(TAG, "Failed to turn laser OFF!!");
        }
    }
}

static void tUiTask(void *pvParameters){

    ESP_LOGI(TAG, "Starting UI task");
    UI_Event_t event = UI_EVENT_INVALID;

    for(;;){
        //Check if event is available
        if(pdTRUE == xQueueReceive(ui_event_queue_handle, &event, UI_EVENT_TIMEOUT_MS/portTICK_PERIOD_MS)){
            //Process incoming event
        }
    }
    vTaskDelete(NULL);
}

/******************************************************************************
*   CallBack Functions implementation
*******************************************************************************/


/******************************************************************************
*   Public Functions Definitions
*******************************************************************************/
UI_Ret_t UI_Init(void){

    //Init led controller
    if(LED_STATUS_OK != LED_InitController()){
        ESP_LOGI(TAG, "Failed to init leds controller");
        return UI_STATUS_ERROR;
    }
    
    //Init trigger driver
    TRIGGER_Config_t trigger_cfg = {
        .active_level = TRIGGER_ACTIVE_LEVEL_HIGH,
        .trigger_gpio = HWI_TRIGGER_IN,
    };

    if(TRIGGER_STATUS_OK != TRIGGER_InitDriver(&trigger_cfg, triggerCallback)){
        ESP_LOGW(TAG, "Failed to init trigger driver");
        return UI_STATUS_ERROR;
    }

    //Create queue
    ui_event_queue_handle = xQueueCreate(UI_EVENT_QUEUE_SIZE, sizeof(UI_Event_t));
    if(ui_event_queue_handle == NULL){
        ESP_LOGE(TAG, "Failed to create UI queue");
        return UI_STATUS_ERROR;
    }

    //Create task
    if(pdTRUE != xTaskCreate(tUiTask,
                             "UI task",
                             2048,
                             NULL,
                             UI_TASK_PRIORITY,
                             &ui_task_handle)){

        ESP_LOGE(TAG, "Failed to create UI task");
        return UI_STATUS_ERROR;
    }

    return UI_STATUS_OK;
}

UI_Ret_t UI_PostEvent(UI_Event_t event, uint32_t timeout_ms){

    if(event >= UI_EVENT_INVALID)   return UI_STATUS_ERROR;

    if(ui_event_queue_handle == NULL)   return UI_STATUS_ERROR;

    if(ESP_OK != xQueueSend(ui_event_queue_handle, &event, timeout_ms/portTICK_PERIOD_MS)){
        ESP_LOGI(TAG, "Failed to post event");
        return UI_STATUS_ERROR;
    }

    return UI_STATUS_OK;
}

/******************************************************************************
*   Interrupts
*******************************************************************************/


