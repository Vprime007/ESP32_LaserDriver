#include <stdint.h>
#include <stdbool.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "esp_log.h"

#include "taskPriority.h"
#include "alarmController.h"

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
static void tAlarmTask(void *pvParameters);

/******************************************************************************
*   Public Variables
*******************************************************************************/


/******************************************************************************
*   Private Variables
*******************************************************************************/
static AlarmStatusCallback_t alarm_callback = NULL;
static AlarmMonitoring_t monitoring_alarm = NULL; 
static ALARM_State_t alarm_state[ALARM_NUMBER_SRC] = {0};

static TaskHandle_t alarm_task_handle = NULL;
static SemaphoreHandle_t alarm_mutex_handle = NULL;

static const char * TAG = "ALARM";

/******************************************************************************
*   Error Check
*******************************************************************************/


/******************************************************************************
*   Private Functions Definitions
*******************************************************************************/
static void tAlarmTask(void *pvParameters){

    ESP_LOGI(TAG, "Starting Alarm task");

    for(;;){

        vTaskDelay(ALARM_MONITORING_PERIOD_MS/portTICK_PERIOD_MS);

        xSemaphoreTake(alarm_mutex_handle, portMAX_DELAY);
        //Scan alarm table and monitor each of them
        for(uint8_t i=0; i<ALARM_NUMBER_SRC; i++){
            ALARM_State_t new_state = monitoring_alarm(i);
            //Check if current state differ from previous one
            if(alarm_state[i] != new_state){
                //Update alarm state
                alarm_state[i] = new_state;
                //Alarm state change callback
                alarm_callback(i, alarm_state[i]);
            }
        }
        xSemaphoreGive(alarm_mutex_handle);

    }
    vTaskDelete(NULL);
}

/******************************************************************************
*   CallBack Functions implementation
*******************************************************************************/


/******************************************************************************
*   Public Functions Definitions
*******************************************************************************/
ALARM_Ret_t ALARM_InitController(AlarmMonitoring_t monitoring_func,
                                 AlarmStatusCallback_t callback){

    if(monitoring_func == NULL){
        ESP_LOGI(TAG, "Failed to init controller: Invalid params");
        return ALARM_STATUS_ERROR;
    }

    //Init alarms states
    for(uint8_t i=0; i<ALARM_NUMBER_SRC; i++){
        alarm_state[i] = ALARM_STATE_INACTIVE;
    }

    //Register callback and monitoring functions
    alarm_callback = callback;
    monitoring_alarm = monitoring_func;

    //Create alarm mutex
    alarm_mutex_handle = xSemaphoreCreateMutex();
    if(alarm_mutex_handle == NULL){
        ESP_LOGI(TAG, "Failed to create alarm mutex");
        return ALARM_STATUS_ERROR;
    }

    //Create alarm task
    if(pdTRUE != xTaskCreate(tAlarmTask,
                             "Alarm task",
                             2048,
                             NULL,
                             ALARM_TASK_PRIORITY,
                             &alarm_task_handle)){

        ESP_LOGE(TAG, "Failed to create alarm task");
        return ALARM_STATUS_ERROR;
    }

    return ALARM_STATUS_OK;
}

ALARM_Ret_t ALARM_GetState(ALARM_Src_t src, ALARM_State_t *pState){

    if(src >= ALARM_SRC_INVALID || pState == NULL){
        ESP_LOGI(TAG, "Failed to get state: Invalid params");
        return ALARM_STATUS_OK;
    }    

    xSemaphoreTake(alarm_mutex_handle, portMAX_DELAY);
    *pState = alarm_state[src];
    xSemaphoreGive(alarm_mutex_handle);

    return ALARM_STATUS_OK;
}

/******************************************************************************
*   Interrupts
*******************************************************************************/


