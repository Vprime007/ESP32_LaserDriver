#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include "esp_log.h"

#include "pwrMonitoring.h"

/******************************************************************************
*   Private Definitions
*******************************************************************************/
#define BUS_VOLTAGE_AVG_SAMPLE          (16)
#define PHASE_CURRENT_AVG_SAMPLE        (16)

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
static SemaphoreHandle_t pwr_mutex_handle = NULL;

static int16_t bus_voltage_10mv = PWR_INVALID_VOLTAGE;
static int32_t cumul_bus_voltage = 0;

static int16_t phase_a_current_10ma = PWR_INVALID_CURRENT;
static int16_t phase_b_current_10ma = PWR_INVALID_CURRENT;
static int32_t cumul_phase_a_current = 0;
static int32_t cumul_phase_b_current = 0;

static const char * TAG = "PWR";

/******************************************************************************
*   Private Functions Definitions
*******************************************************************************/


/******************************************************************************
*   Public Functions Definitions
*******************************************************************************/
PWR_Ret_t PWR_InitMonitoring(void){

    ESP_LOGI(TAG, "Monitoring initialization");

    //Init global variables
    bus_voltage_10mv = PWR_INVALID_VOLTAGE;
    cumul_bus_voltage = 0;

    phase_a_current_10ma = PWR_INVALID_CURRENT;
    phase_b_current_10ma = PWR_INVALID_CURRENT;
    cumul_phase_a_current = 0;
    cumul_phase_b_current = 0;

    //Create mutex
    pwr_mutex_handle = xSemaphoreCreateMutex();
    if(pwr_mutex_handle == NULL){

        ESP_LOGE(TAG, "Failed to create PWR mutex");
        return PWR_MONITORING_STATUS_ERROR;
    }

    return PWR_MONITORING_STATUS_OK;
}

PWR_Ret_t PWR_ProcessRawMeasurement(uint8_t *pMeas_buf, uint32_t size){

    if(pMeas_buf == NULL || size == 0){
        ESP_LOGI(TAG, "Invalid measurement buffer....")
        return PWR_MONITORING_STATUS_ERROR;
    }

    xSemaphoreTake(pwr_mutex_handle, portMAX_DELAY);
    //Process raw measurement and update cumuls

    xSemaphoreGive(pwr_mutex_handle);
    
    return PWR_MONITORING_STATUS_OK;
}

PWR_Ret_t PWR_GetBusVoltage(int16_t *pVoltage){

    if(pVoltage == NULL){
        ESP_LOGI(TAG, "Invalid voltage buffer");
        return PWR_MONITORING_STATUS_ERROR;
    }

    xSemaphoreTake(pwr_mutex_handle, portMAX_DELAY);
    *pVoltage = bus_voltage_10mv;
    xSemaphoreGive(pwr_mutex_handle);

    return PWR_MONITORING_STATUS_OK;
}

PWR_Ret_t PWR_GetPhaseACurrent(int16_t *pCurrent){

    if(pCurrent == NULL){
        ESP_LOGI(TAG, "Invalid current buffer");
        return PWR_MONITORING_STATUS_ERROR;
    }

    xSemaphoreTake(pwr_mutex_handle, portMAX_DELAY);
    *pCurrent = phase_a_current_10ma;
    xSemaphoreGive(pwr_mutex_handle);

    return PWR_MONITORING_STATUS_OK;
}

PWR_Ret_t PWR_GetPhaseBCurrent(int16_t *pCurrent){

    if(pCurrent == NULL){
        ESP_LOGI(TAG, "Invalid current buffer");
        return PWR_MONITORING_STATUS_ERROR;
    }

    xSemaphoreTake(pwr_mutex_handle, portMAX_DELAY);
    *pCurrent = phase_b_current_10ma;
    xSemaphoreGive(pwr_mutex_handle);

    return PWR_MONITORING_STATUS_OK;
}

/******************************************************************************
*   Interrupts
*******************************************************************************/

