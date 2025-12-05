#include <stdint.h>
#include <stdbool.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "esp_log.h"

#include "temperatureMonitoring.h"
#include "pwrMonitoring.h"

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
static AlarmStateCallback_t alarm_callback = NULL;
static ALARM_State_t alarm_state[ALARM_NUMBER_SRC] = {0};

static int16_t temp_threshold;
static int16_t temp_release;
static int16_t volt_threshold;
static int16_t volt_release;

static TaskHandle_t alarm_task_handle = NULL;
static SemaphoreHandle_t alarm_mutex_handle = NULL;

static const char * TAG = "ALARM";

/******************************************************************************
*   Error Check
*******************************************************************************/


/******************************************************************************
*   Private Functions Definitions
*******************************************************************************/
/***************************************************************************//*!
*  \brief Alarm controller task.
*
*   This function is the alarm controller task.
*   
*   Preconditions: None.
*
*   Side Effects: None.
*
*   \param[in]  pvParameters            User parameters.
*
*   \return     None.
*
*******************************************************************************/
static void tAlarmTask(void *pvParameters){

    ESP_LOGI(TAG, "Starting Alarm task");

    for(;;){

        vTaskDelay(ALARM_MONITORING_PERIOD_MS/portTICK_PERIOD_MS);

        xSemaphoreTake(alarm_mutex_handle, portMAX_DELAY);

        //Get load temp value 
        int16_t load_temp = TEMP_ERROR_INVALID;
        if(TEMP_STATUS_OK != TEMP_GetTemperature(TEMP_SENSOR_ID_LOAD, &load_temp)){
            ESP_LOGI(TAG, "Failed to get load temp");
        }
        else{
            //Process load temp value
            
        } 

        //Get phase A temp value
        int16_t pA_temp = TEMP_ERROR_INVALID;
        if(TEMP_STATUS_OK != TEMP_GetTemperature(TEMP_SENSOR_ID_PHASE_A, &pA_temp)){
            ESP_LOGI(TAG, "Failed to get phase A temp");
        }
        else{
            //process phase A temp value
        
        }

        //Get phase B temp value
        int16_t pB_temp = TEMP_ERROR_INVALID;
        if(TEMP_STATUS_OK != TEMP_GetTemperature(TEMP_SENSOR_ID_PHASE_B, &pB_temp)){
            ESP_LOGI(TAG, "Failed to get phase B temp");
        }
        else{
            //process phase B temp value
        
        }

        //Get Vbus voltage
        int16_t bus_volt_10mv = PWR_INVALID_VOLTAGE;
        if(PWR_MONITORING_STATUS_OK != PWR_GetBusVoltage(&bus_volt_10mv)){
            ESP_LOGI(TAG, "Failed to get bus voltage");
        }
        else{
            //process new Vbus voltage
        
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
/***************************************************************************//*!
*  \brief Alarm Controller initialization.
*
*   This function is used to initialize the alarm controller module.
*   The callback function will be called when a alarm state change occur.
*   
*   Preconditions: None.
*
*   Side Effects: None.
*
*   \param[in]  pTemp_cfg           Pointer to the temp configuration.
*   \param[in]  pVolt_cfg           Pointer to the volt configuration.
*   \param[in]  callback            Alarm state change callback.
*
*   \return     Operation status
*
*******************************************************************************/
ALARM_Ret_t ALARM_InitController(ALARM_Temp_Config_t *pTemp_cfg,
                                 ALARM_Volt_Config_t *pVolt_cfg,
                                 AlarmStateCallback_t callback){

    if(pTemp_cfg == NULL || pVolt_cfg == NULL || callback == NULL){
        ESP_LOGI(TAG, "Failed to init controller: Invalid params");
        return ALARM_STATUS_ERROR;
    }

    //Init alarm thresholds and releases levels
    temp_threshold = pTemp_cfg->temp_threshold_10mC;
    temp_release = pTemp_cfg->temp_release_10mc;
    volt_threshold = pVolt_cfg->volt_threshold_10mv;
    volt_release = pVolt_cfg->volt_release_10mv;

    //Register callback
    alarm_callback = callback;

    //Init alarms states
    for(uint8_t i=0; i<ALARM_NUMBER_SRC; i++){
        alarm_state[i] = ALARM_STATE_INACTIVE;
    }

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

/***************************************************************************//*!
*  \brief Get alarm state.
*
*   This function is used to get the current state of an alarm.
*   
*   Preconditions: None.
*
*   Side Effects: None.
*
*   \param[in]  src                 Alarm source.
*   \param[out] pState              Pointer to store the state.
*
*   \return     Operation status
*
*******************************************************************************/
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

/***************************************************************************//*!
*  \brief Set temperature alarm level.
*
*   This function is used to set the temperature threshold and 
*   release level.
*   
*   Preconditions: None.
*
*   Side Effects: None.
*
*   \param[in]  pTemp_cfg           Pointer to the temperature config.
*
*   \return     Operation status
*
*******************************************************************************/
ALARM_Ret_t ALARM_SetTempLevel(ALARM_Temp_Config_t *pTemp_cfg){

    xSemaphoreTake(alarm_mutex_handle, portMAX_DELAY);
    temp_threshold = pTemp_cfg->temp_threshold_10mC;
    temp_release = pTemp_cfg->temp_release_10mc;
    xSemaphoreGive(alarm_mutex_handle);

    return ALARM_STATUS_OK;
}

/***************************************************************************//*!
*  \brief Get temperature alarm level.
*
*   This function is used to get the temperature threshold and 
*   release level.
*   
*   Preconditions: None.
*
*   Side Effects: None.
*
*   \param[in]  pTemp_cfg           Pointer to store the temperature config.
*
*   \return     Operation status
*
*******************************************************************************/
ALARM_Ret_t ALARM_GetTempLevel(ALARM_Temp_Config_t *pTemp_cfg){

    if(pTemp_cfg == NULL)   return ALARM_STATUS_ERROR;

    xSemaphoreTake(alarm_mutex_handle, portMAX_DELAY);
    pTemp_cfg->temp_threshold_10mC = temp_threshold;
    pTemp_cfg->temp_release_10mc = temp_release;
    xSemaphoreGive(alarm_mutex_handle);

    return ALARM_STATUS_OK;
}

/***************************************************************************//*!
*  \brief Set voltage level.
*
*   This function is used to set the voltage threshold and 
*   release level.
*   
*   Preconditions: None.
*
*   Side Effects: None.
*
*   \param[in]  pTemp_cfg           Pointer to the voltage config.
*
*   \return     Operation status
*
*******************************************************************************/
ALARM_Ret_t ALARM_SetVoltLevel(ALARM_Volt_Config_t *pVolt_cfg){

    if(pVolt_cfg == NULL)   return ALARM_STATUS_ERROR;

    xSemaphoreTake(alarm_mutex_handle, portMAX_DELAY); 
    volt_threshold = pVolt_cfg->volt_threshold_10mv;
    volt_release = pVolt_cfg->volt_release_10mv;
    xSemaphoreGive(alarm_mutex_handle);

    return ALARM_STATUS_OK;
}

/***************************************************************************//*!
*  \brief Get voltage alarm level.
*
*   This function is used to get the voltage threshold and 
*   release level.
*   
*   Preconditions: None.
*
*   Side Effects: None.
*
*   \param[in]  pTemp_cfg           Pointer to store the voltage config.
*
*   \return     Operation status
*
*******************************************************************************/
ALARM_Ret_t ALARM_GetVoltLevel(ALARM_Volt_Config_t *pVolt_cfg){

    if(pVolt_cfg == NULL)   return ALARM_STATUS_ERROR;

    xSemaphoreTake(alarm_mutex_handle, portMAX_DELAY);
    pVolt_cfg->volt_threshold_10mv = volt_threshold;
    pVolt_cfg->volt_release_10mv = volt_release;
    xSemaphoreGive(alarm_mutex_handle);

    return ALARM_STATUS_OK;
}

/******************************************************************************
*   Interrupts
*******************************************************************************/


