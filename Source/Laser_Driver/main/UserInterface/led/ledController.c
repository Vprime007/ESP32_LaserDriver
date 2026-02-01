/******************************************************************************
*   Includes
*******************************************************************************/
#include <stdbool.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"

#include "ledController.h"
#include "sequencer.h"
#include "hwi.h"


/******************************************************************************
*   Private Definitions
*******************************************************************************/


/******************************************************************************
*   Private Macros
*******************************************************************************/


/******************************************************************************
*   Private Data Types
*******************************************************************************/


/******************************************************************************
*   Private Functions Declaration
*******************************************************************************/
static void tSequencerTask(void *pvParameters);
static void tLedTask(void *pvParameters);

static void userLedTimerCallback(TimerHandle_t xTimer);

static void processUserLedEvent(void);

/******************************************************************************
*   Public Variables
*******************************************************************************/


/******************************************************************************
*   Private Variables
*******************************************************************************/
//Boot led sequence
const SEQUENCE_t seq_boot = {
    .init_time_off = SEQUENCER_MS_TO_TIC(500),
    .time_on = SEQUENCER_MS_TO_TIC(5000),
    .time_off = SEQUENCER_MS_TO_TIC(100),
    .nb_repeat = 0,
};

//Always ON led sequence
const SEQUENCE_t seq_always_on = {
    .init_time_off = SEQUENCER_MS_TO_TIC(250),
    .time_on = SEQUENCE_ACTIVE_FOREVER,
    .time_off = 0,
};

//Always OFF led sequence
const SEQUENCE_t seq_always_off = {
    .init_time_off = SEQUENCER_MS_TO_TIC(250),
    .time_on = 0,
    .time_off = SEQUENCE_ACTIVE_FOREVER,
};

static SemaphoreHandle_t led_mutex_handle = NULL;
static TaskHandle_t seq_task_handle = NULL;
static TaskHandle_t led_task_handle = NULL;

static LED_Handle_t user_led_handle = LED_DRIVER_HANDLE_INVALID;
static LED_Pattern_t user_led_current_pattern = LED_PATTERN_INVALID;
static LED_Pattern_t user_led_buffered_pattern = LED_PATTERN_INVALID;
static TimerHandle_t user_led_timer_handle = NULL;
static SemaphoreHandle_t user_led_semph_handle = NULL;


/******************************************************************************
*   Private Functions Definitions
*******************************************************************************/
/***************************************************************************//*!
*  \brief User led timer callback
*
*   This function is called after the user led timeout occur.
*   
*   Preconditions: None.
*
*   Side Effects: None.
*
*   \param[in]  xTimer          timer handle
*
*******************************************************************************/
static void userLedTimerCallback(TimerHandle_t xTimer){

    xSemaphoreGive(user_led_timer_handle);
}

/***************************************************************************//*!
*  \brief Sequencer task
*
*   This function is the sequencer task.
*   
*   Preconditions: None.
*
*   Side Effects: None.
*
*   \param[in]  pvParameters            task parameters
*
*******************************************************************************/
static void tSequencerTask(void *pvParameters){

    for(;;){

        vTaskDelay(SEQUENCER_TIC_PERIOD_MS/portTICK_PERIOD_MS);
        SEQUENCER_Tic();
    }
    vTaskDelete(NULL);
}

/***************************************************************************//*!
*  \brief Led task
*
*   This function is the Led task. It managed all led patterns and transitions.
*   
*   Preconditions: None.
*
*   Side Effects: None.
*
*   \param[in]  pvParameters            task parameters
*
*******************************************************************************/
static void tLedTask(void *pvParameters){

    for(;;){
        
        //Check if there is red led event to process
        if(pdPASS == xSemaphoreTake(user_led_semph_handle, 0)){
            processUserLedEvent();
        }

        vTaskDelay(10/portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

/***************************************************************************//*!
*  \brief Process user led event
*
*   This function is used to process user led events.
*   
*   Preconditions: None.
*
*   Side Effects: None.
*
*******************************************************************************/
static void processUserLedEvent(void){

    LED_Pattern_t pattern = LED_PATTERN_INVALID;

    xSemaphoreTake(led_mutex_handle, portMAX_DELAY);
    pattern = user_led_current_pattern;
    xSemaphoreGive(led_mutex_handle);

    switch(pattern){
        case LED_PATTERN_BOOT:
        {
            xSemaphoreTake(led_mutex_handle, portMAX_DELAY);
            user_led_current_pattern = LED_PATTERN_INVALID;
            xSemaphoreGive(led_mutex_handle);  
            
            SEQUENCER_DoSequence(SEQUENCE_ID_USER_LED, &seq_boot);

            //Schedule led update after the sequence
            xTimerStop(user_led_timer_handle, 10/portTICK_PERIOD_MS);
            xTimerChangePeriod(user_led_timer_handle, (5000+500)/portTICK_PERIOD_MS, 10/portTICK_PERIOD_MS);
            xTimerStart(user_led_timer_handle, 10/portTICK_PERIOD_MS);
        }
        break;

        case LED_PATTERN_INVALID:
        default:
        {
            SEQUENCER_DoSequence(SEQUENCE_ID_USER_LED, &seq_always_off);
        }
        break;
    }
}

/******************************************************************************
*   Public Functions Definitions
*******************************************************************************/
/***************************************************************************/ /*!
*  \brief LED controller initialization.
*
*   Function to initialized the LED controller module.
*
*   Preconditions: None.
*
*   Side Effects: None.
*   
*   \return operation status
*
*******************************************************************************/
LED_Ret_t LED_InitController(void){

    //Create led mutex
    led_mutex_handle = xSemaphoreCreateMutex();
    if(led_mutex_handle == NULL){
        return LED_STATUS_ERROR;
    }

    //create led binary semaphores
    user_led_semph_handle = xSemaphoreCreateBinary();
    if(user_led_semph_handle == NULL){
        return LED_STATUS_ERROR;
    }

    //create leds timers
    user_led_timer_handle = xTimerCreate("User Led timer",
                                         1000/portTICK_PERIOD_MS,
                                         pdFALSE,
                                         (void*)0,
                                         userLedTimerCallback);

    if(user_led_timer_handle == NULL){
        return LED_STATUS_ERROR;
    }

    LDRV_CFG_Single_Pwm_Config_t red_led_cfg = {
        .active_level = LDRV_CFG_ACTIVE_HIGH,
        .gpio_num = HWI_USER_LED_GPIO,
        .led_channel = LEDC_CHANNEL_0,
        .led_timer = LEDC_TIMER_1,
    };
    if(LDRV_STATUS_OK != LDRV_AddLedSinglePwm(red_led_cfg, &user_led_handle)){
        return LED_STATUS_ERROR;
    }    

    user_led_current_pattern = LED_PATTERN_INVALID;
    user_led_buffered_pattern = LED_PATTERN_INVALID;

    //create sequencer task
    if(pdTRUE != xTaskCreate(tSequencerTask,
                             "Seq Task",
                             2048,
                             NULL,
                             5,
                             &seq_task_handle)){
        
        return LED_STATUS_ERROR;
    }

    //create leds task
    if(pdTRUE != xTaskCreate(tLedTask,
                             "Led task",
                             2048,
                             NULL,
                             6,
                             &led_task_handle)){

        return LED_STATUS_ERROR;
    }

    return LED_STATUS_OK;
}

/***************************************************************************/ /*!
*  \brief Start Led pattern
*
*   Function used to start a Led pattern.
*
*   Preconditions: None.
*
*   Side Effects: None.
*   
*   \param[in]  pattern         Led pattern to start   
*
*   \return operation status
*
*******************************************************************************/
LED_Ret_t LED_StartPattern(LED_Pattern_t pattern){

    if(pattern >= LED_PATTERN_INVALID){
        return LED_STATUS_ERROR;
    }
    
    switch(pattern){

        case LED_PATTERN_BOOT:
        {
            xSemaphoreTake(led_mutex_handle, portMAX_DELAY);
            user_led_current_pattern = LED_PATTERN_BOOT;
            user_led_buffered_pattern = LED_PATTERN_BOOT;
            xSemaphoreGive(led_mutex_handle);

            //Stop any running timer
            xTimerStop(user_led_timer_handle, 10/portTICK_PERIOD_MS);

            //Notify the task to apply new pattern
            xSemaphoreGive(user_led_semph_handle);
        }
        break;

        case LED_PATTERN_INVALID:
        default:
        {
            //Do notthing...
        }
        break;
    }

    return LED_STATUS_OK;
}

/***************************************************************************/ /*!
*  \brief Stop Led pattern
*
*   Function used to stop an active Led pattern.
*
*   Preconditions: None.
*
*   Side Effects: None.
*   
*   \param[in]  pattern         Led pattern to stop  
*
*   \return operation status
*
*******************************************************************************/
LED_Ret_t LED_StopPattern(LED_Pattern_t pattern){

    if(pattern >= LED_PATTERN_INVALID){
        return LED_STATUS_ERROR;
    }

    return LED_STATUS_OK;
}

/***************************************************************************/ /*!
*  \brief Get led handle
*
*   This function is used to get the led driver handle of a led entity.
*
*   Preconditions: None.
*
*   Side Effects: None.
*
*   \param[in]  led_id              Led ID
*   \param[out] pHandle             Pointer to store the correspondig led handle.
*
*   \return     Operation status
*
*******************************************************************************/
LED_Ret_t LED_GetLedHandle(LED_Ctrl_Id_t led_id, LED_Handle_t *pHandle){

    if((led_id >= LED_CTRL_ID_INVALID) || (pHandle == NULL)){

        return LED_STATUS_ERROR;
    }

    xSemaphoreTake(led_mutex_handle, portMAX_DELAY);
    
    switch(led_id){
        case LED_CTRL_ID_USER:
        {
            *pHandle = user_led_handle;
        }
        break;

        default:
        {
            //Do nothing...
        }
        break;
    }
    
    xSemaphoreGive(led_mutex_handle);

    return LED_STATUS_OK;
}

/******************************************************************************
*   Interrupts
*******************************************************************************/


