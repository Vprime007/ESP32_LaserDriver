#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "taskPriority.h"
#include "bsp.h"
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


