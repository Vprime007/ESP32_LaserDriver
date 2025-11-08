#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include "esp_log.h"

#include "temperatureMonitoring.h"

/******************************************************************************
*   Private Definitions
*******************************************************************************/
#define LOG_LOCAL_LEVEL                     (ESP_LOG_INFO)

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
static SemaphoreHandle_t temp_mutex_handle = NULL;

static int16_t temp_load = TEMP_ERROR_INVALID;
static int32_t cumul_temp_load = 0;

static int16_t temp_phase_a = TEMP_ERROR_INVALID;
static int32_t cumul_temp_phase_a = 0;

static int16_t temp_phase_b = TEMP_ERROR_INVALID;
static int32_t cumul_temp_phase_b = 0;

static const char * TAG = "TEMPERATURE";

/******************************************************************************
*   Private Functions Definitions
*******************************************************************************/


/******************************************************************************
*   Public Functions Definitions
*******************************************************************************/
/***************************************************************************//*!
*  \brief Temperature Monitoring initialization.
*
*   This function is used to initialize the temperature monitoring
*   module.
*   
*   Preconditions: None.
*
*   Side Effects: None.
*
*   \return     Operation status
*
*******************************************************************************/
TEMP_Ret_t TEMP_InitMonitoring(void){

    //Create mutex
    temp_mutex_handle = xSemaphoreCreateMutex();
    if(temp_mutex_handle == NULL){
        ESP_LOGE(TAG, "Failed to create temperature mutex");
        return TEMP_STATUS_ERROR;
    }

    //Init global variables
    temp_load = TEMP_ERROR_INVALID;
    cumul_temp_load = 0;

    temp_phase_b = TEMP_ERROR_INVALID;
    cumul_temp_phase_a = 0;

    temp_phase_b = TEMP_ERROR_INVALID;
    cumul_temp_phase_b = 0;

    return TEMP_STATUS_OK;
}

/***************************************************************************//*!
*  \brief Process Raw temperature measurements.
*
*   This function is used to process raw temperature measurement from the 
*   ADC
*   
*   Preconditions: None.
*
*   Side Effects: None.
*
*   \param[in]  pMeas_buf           Raw measurements buffer.
*   \param[in]  size                Buffer size (in sample)   
*
*   \return     Operation status
*
*******************************************************************************/
TEMP_Ret_t TEMP_ProcessRawMeasurement(uint8_t *pMeas_buf, uint32_t size){

    if((pMeas_buf == NULL) || (size == 0)){
        ESP_LOGI(TAG, "Invalid buffer");
        return TEMP_STATUS_ERROR;
    }

    return TEMP_STATUS_OK;
}

/***************************************************************************//*!
*  \brief Get temperature.
*
*   This function is used get the latest temperature value of a temperature 
*   sensor.
*   
*   Preconditions: None.
*
*   Side Effects: None.
*
*   \param[in]  sensor_id           Sensor ID.
*   \param[in]  pTemperature        Pointer to store temperature value.  
*
*   \return     Operation status
*
*******************************************************************************/
TEMP_Ret_t TEMP_GetTemperature(TEMP_Sensor_Id_t sensor_id, int16_t *pTemperature){

    if((sensor_id >= TEMP_SENSOR_ID_INVALID) || (pTemperature == NULL)){

        ESP_LOGI(TAG, "Invalid parameters");
        return TEMP_STATUS_ERROR;
    }

    int16_t current_temp = TEMP_ERROR_INVALID;

    xSemaphoreTake(temp_mutex_handle, portMAX_DELAY);

    switch(sensor_id){
        case TEMP_SENSOR_ID_LOAD:
        {
            current_temp = temp_load;
        }
        break;

        case TEMP_SENSOR_ID_PHASE_A:
        {
            current_temp = temp_phase_a;
        }
        break;

        case TEMP_SENSOR_ID_PHASE_B:
        {
            current_temp = temp_phase_b;
        }
        break;

        default:
        {
            //Do nothing...
        }
        break;
    }

    xSemaphoreGive(temp_mutex_handle);

    *pTemperature = current_temp;

    return TEMP_STATUS_OK;
}

/******************************************************************************
*   Interrupts
*******************************************************************************/


