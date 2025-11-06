#ifndef __SHELL_COM_UART_H
#define __SHELL_COM_UART_H

#include <stdint.h>
#include "driver/uart.h"

/******************************************************************************
*   Public Definitions
*******************************************************************************/


/******************************************************************************
*   Public Macros
*******************************************************************************/


/******************************************************************************
*   Public Data Types
*******************************************************************************/
typedef struct SHCOM_Config_s{
    uint8_t tx_gpio;
    uint8_t rx_gpio;
    uint32_t baudrate;
    uart_port_t port;
}SHCOM_Config_t;

typedef enum SHCOM_Ret_e{
    SHCOM_STATUS_ERROR,
    SHCOM_STATUS_OK,
}SHCOM_Ret_t;

/******************************************************************************
*   Public Variables
*******************************************************************************/


/******************************************************************************
*   Error Check
*******************************************************************************/


/******************************************************************************
*   Public Functions Declarations
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
SHCOM_Ret_t SHCOM_InitCom(SHCOM_Config_t *pConfig);

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
SHCOM_Ret_t SHCOM_PrintCharacter(char c);

#endif//__SHELL_COM_UART_H