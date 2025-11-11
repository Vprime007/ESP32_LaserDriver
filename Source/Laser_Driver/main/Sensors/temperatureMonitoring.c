#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include "esp_log.h"

#include "tempConversion.h"
#include "temperatureMonitoring.h"

/******************************************************************************
*   Private Definitions
*******************************************************************************/
#define TEMP_CONV_ADC_TOLERANCE             (10)//Adc count

#define LOG_LOCAL_LEVEL                     (ESP_LOG_INFO)

/******************************************************************************
*   Private Macros
*******************************************************************************/
#define NB_ELEMENTS(x)                      (sizeof(x)/sizeof(x[0]))

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

static TEMP_Conv_t conv_table[] = {
    //-40               //-37.5             //-35               //-32.5
    {199, -4000},       {227, -3750},       {258, -3500},       {292, -3250},
    //-30               //-27.5             //-25               //-22.5
    {330, -3000},       {371, -2750},       {416, -2500},       {465, -2250},
    //-20               //-17.5             //-15               //-12.5
    {519, -2000},       {576, -1750},       {638, -1500},       {704, -1250},
    //-10               //-7.5              //-5                //-2.5
    {774, -1000},       {848, -750},        {926, -500},        {1008, -250},
    //0,                //2.5               //5                 //7.5
    {1093, 0},          {1182, 250},        {1273, 500},        {1366, 750},
    //10                //12.5              //15                //17.5
    {1462, 1000},       {1559, 1250},       {1656, 1500},       {1755, 1750},
    //20                //22.5              //25                //27.5
    {1853, 2000},       {1951, 2250},       {2048, 2500},       {2143, 2750},
    //30                //32.5              //35                //37.5
    {2237, 3000},       {2329, 3250},       {2418, 3500},       {2505, 3750},
    //40                //42.5              //45                //47.5
    {2589, 4000},       {2670, 4250},       {2748, 4500},       {2822, 4750},
    //50                //52.5              //55                //57.5
    {2894, 5000},       {2962, 5250},       {3027, 5500},       {3088, 5750},
    //60                //62.5              //65                //67.5
    {3147, 6000},       {3202, 6250},       {3254, 6500},       {3304, 6750},
    //70                //72.5              //75                //77.5
    {3350, 7000},       {3394, 7250},       {3435, 7500},       {3474, 7750},
    //80                //82.5              //85                //87.5
    {3511, 8000},       {3545, 8250},       {3577, 8500},       {3607, 8750},
    //90                //92.5              //95                //97.5
    {3635, 9000},       {3662, 9250},       {3687, 9500},       {3710, 9750},
    //100
    {3732, 10000},
};

static TEMP_Conv_Config_t conv_config = {
    .pConvTable = conv_table,
    .adc_tolerance = TEMP_CONV_ADC_TOLERANCE,
    .table_size = NB_ELEMENTS(conv_table),
};

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


