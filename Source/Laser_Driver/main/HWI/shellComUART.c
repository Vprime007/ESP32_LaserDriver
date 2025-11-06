#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "myShell.h"
#include "shellComUART.h"
#include "taskPriority.h"

/******************************************************************************
*   Private Definitions
*******************************************************************************/
#define UART_EVENT_QUEUE_SIZE           (16)
#define UART_RX_TIMEOUT_MS              (250)

/******************************************************************************
*   Private Macros
*******************************************************************************/


/******************************************************************************
*   Private Data Types
*******************************************************************************/


/******************************************************************************
*   Private Functions Declaration
*******************************************************************************/
static void tUartListenerTask(void *pvParameters);

/******************************************************************************
*   Public Variables
*******************************************************************************/


/******************************************************************************
*   Private Variables
*******************************************************************************/
static QueueHandle_t uart_event_queue_handle = NULL;
static TaskHandle_t uart_listener_task_handle = NULL;

static uart_port_t uart_port;
static uint32_t uart_tx_gpio;
static uint32_t uart_rx_gpio;

/******************************************************************************
*   Error Check
*******************************************************************************/


/******************************************************************************
*   Private Functions Definitions
*******************************************************************************/
/***************************************************************************//*!
*  \brief Uart listener task.
*
*   This function is the UART event listener task.
*
*   Preconditions:  None.
*
*******************************************************************************/
static void tUartListenerTask(void *pvParameters){

    uart_event_t event;
    size_t buffer_len;
    uint8_t read_len;
    uint8_t incoming_byte;

    for(;;){

        while(pdTRUE == xQueueReceive(uart_event_queue_handle, 
                                      &event, 
                                      0)){

            switch(event.type){
                
                case UART_DATA:
                {
                    uart_get_buffered_data_len(uart_port, &buffer_len);
                }
                break;

                case UART_BUFFER_FULL:
                case UART_FIFO_OVF:
                {
                    uart_get_buffered_data_len(uart_port, &buffer_len);
                    uart_flush_input(uart_port);
                }
                break;

                case UART_BREAK:
                case UART_FRAME_ERR:
                case UART_PATTERN_DET:
                default:
                {
                    //Do nothing...
                }
                break;
            }

        }

        read_len = uart_read_bytes(uart_port, &incoming_byte, 1, UART_RX_TIMEOUT_MS/portTICK_PERIOD_MS);
        if(read_len > 0){
            SHELL_RecvChar((char)incoming_byte);
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
/***************************************************************************//*!
*  \brief Shell communication peripheral initialization.
*
*   This function is use to initialize required peripherals for the Shell
*   communication.
*
*   Preconditions:  None.
*
*	\param[in]  pConfig     Pointer to com config.
*
*	\return		Operation status.
*
*******************************************************************************/
SHCOM_Ret_t SHCOM_InitCom(SHCOM_Config_t *pConfig){

    //Init uart port
    uart_config_t config = {
        .baud_rate = pConfig->baudrate,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

    if(ESP_OK != uart_driver_install(pConfig->port,
                                     2*UART_HW_FIFO_LEN(pConfig->port),
                                     2*UART_HW_FIFO_LEN(pConfig->port),
                                     UART_EVENT_QUEUE_SIZE,
                                     &uart_event_queue_handle,
                                     0)){

        return SHCOM_STATUS_ERROR;
    }

    if(ESP_OK != uart_param_config(pConfig->port, &config)){
        return SHCOM_STATUS_ERROR;
    }

    if(ESP_OK != uart_set_pin(pConfig->port, 
                              pConfig->tx_gpio, 
                              pConfig->rx_gpio, 
                              UART_PIN_NO_CHANGE, 
                              UART_PIN_NO_CHANGE)){

        return SHCOM_STATUS_ERROR;
    }

    uart_flush(pConfig->port);

    //Store uart port and pins
    uart_port = pConfig->port;
    uart_tx_gpio = pConfig->tx_gpio;
    uart_rx_gpio = pConfig->rx_gpio;

    //Create uart listener task
    if(pdTRUE != xTaskCreate(tUartListenerTask,
                             "Listener task",
                             2048,
                             NULL,
                             SHCOM_TASK_PRIORITY,
                             &uart_listener_task_handle)){

        return SHCOM_STATUS_ERROR;
    }

    return SHCOM_STATUS_OK;
}

/***************************************************************************//*!
*  \brief Shell communication print character.
*
*   This function is use by the shell to print character out.
*
*   Preconditions:  None.
*
*	\param[in]  c                   Character to print.
*
*	\return		Operation status.
*
*******************************************************************************/
SHCOM_Ret_t SHCOM_PrintCharacter(char c){

    uart_write_bytes(uart_port, &c, 1);

    return SHCOM_STATUS_OK;
}

/******************************************************************************
*   Interrupts
*******************************************************************************/

