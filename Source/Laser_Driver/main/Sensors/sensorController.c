#include <stdint.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "esp_log.h"

#include "hwi.h"
#include "taskPriority.h"
#include "adcController.h"
#include "sensorController.h"

/******************************************************************************
*   Private Definitions
*******************************************************************************/
#define LOG_LOCAL_LEVEL             ESP_LOG_INFO

/******************************************************************************
*   Private Macros
*******************************************************************************/


/******************************************************************************
*   Private Data Types
*******************************************************************************/
typedef enum SENSOR_Step_e{
    SENSOR_STEP_IDLE,
    SENSOR_STEP_EN_SENSOR,
    SENSOR_STEP_START_ADC,
    SENSOR_STEP_PROCESS_TEMP,
    SENSOR_STEP_PROCESS_PWR,

    SENSOR_STEP_INVALID,
}SENSOR_Step_t;

/******************************************************************************
*   Private Functions Declaration
*******************************************************************************/
static void tSensorTask(void *pvParameters);

static void enableTempSensors(void);
static void disableTempSensors(void);

/******************************************************************************
*   Public Variables
*******************************************************************************/


/******************************************************************************
*   Private Variables
*******************************************************************************/
static TaskHandle_t sensor_task_handle = NULL;

static SENSOR_Step_t sensor_step = SENSOR_STEP_INVALID;

static const char * TAG = "SENSOR";

/******************************************************************************
*   Private Functions Definitions
*******************************************************************************/
static void enableTempSensors(void){

    gpio_set_level(HWI_SENSOR_EN_GPIO, 1);
}

static void disableTempSensors(void){

    gpio_set_level(HWI_SENSOR_EN_GPIO, 0);
}

static void tSensorTask(void *pvParameters){

    ESP_LOGI(TAG, "Starting sensor task");

    for(;;){

        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

/******************************************************************************
*   Public Functions Definitions
*******************************************************************************/
SENSOR_Ret_t SENSOR_InitController(void){

    //Init sensor step
    sensor_step = SENSOR_STEP_IDLE;

    //Init sensors en gpio
    gpio_config_t gpio_cfg = {
        .mode = GPIO_MODE_OUTPUT,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
        .pin_bit_mask = (1ULL << HWI_SENSOR_EN_GPIO),
    };
    if(ESP_OK != gpio_config(&gpio_cfg)){

        ESP_LOGE(TAG, "Failed to init sensors gpio");
        return SENSOR_STATUS_ERROR;
    }

    //Create task
    if(pdTRUE != xTaskCreate(tSensorTask,
                             "Sensor Task",
                             2048,
                             NULL,
                             SENSOR_TASK_PRIORITY,
                             &sensor_task_handle)){

        ESP_LOGE(TAG, "Failed to create sensor task");
        return SENSOR_STATUS_ERROR;
    }


    return SENSOR_STATUS_OK;
}

/******************************************************************************
*   Interrupts
*******************************************************************************/


