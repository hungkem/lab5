/*
 * uart.c
 *
 *  Created on: Dec 10, 2024
 *      Author: PC
 */


#include "uart.h"
#include "global.h"
#include "software_timer.h"
#include <stdio.h>
#include "main.h"
#include"error.h"
uint32_t ADC_value = 0;
char response[RESPONSE_LENGTH];
uint8_t data[] = "hello world\r\n";
//flag state
int isRST = 0;
int isOK = 0;

	void uart_communication_fsm()
	{
		switch(uartState)
		{
			case UART_IDLE:
				break;
			case UART_RST:
				ADC_value = HAL_ADC_GetValue(&hadc1);
				HAL_UART_Transmit(&huart2, (void *)response, sprintf(response, "!ADC=%lu#\r\n", ADC_value), 1000);
				uartState = UART_RESPONSE;
				break;
			case UART_RESPONSE:
				set_timer(1, TIME_FOR_3S);
				uartState = UART_WAIT_OK;
				break;
			case UART_WAIT_OK:
				if(is_time_expired(1))
				{
					set_timer(1, TIME_FOR_3S);
					HAL_UART_Transmit(&huart2, (void *)response, sprintf(response, "!ADC=%lu#\r\n", ADC_value), 1000);
				}

				if(isOK)
				{
					if(isRST) uartState = UART_END;
					else      uartState = UART_IDLE;
					isOK  = 0;
					isRST = 0;
				}
				break;
			case UART_END:
				uartState = UART_IDLE;
				HAL_UART_Transmit(&huart2, (void *)response, sprintf(response, "END\r\n"), 1000);
				break;
			case UART_ERROR:
				uartState = UART_IDLE;
				HAL_UART_Transmit(&huart2, (void *)response, sprintf(response, "WRONG\r\n"), 1000);
				break;
			default: break;

		}
	}

	void command_parser_fsm()
	{
		switch(cmdState)
		{
			case CMD_IDLE:
				if(cmdBuffer[cmd_content_index-1] == '!')
				{
					cmdState = CMD_EXCLAMATION;
				}
				else if(cmdBuffer[cmd_content_index-1] == '\r' || cmdBuffer[cmd_content_index-1] == '\n')
				{
					cmdState = CMD_IDLE;
					uartState = UART_ERROR;
					cmd_content_index = 0;
				}
				break;
			case CMD_EXCLAMATION:
				if(cmdBuffer[cmd_content_index-1] == 'R')
				{
					cmdState = CMD_R;
				}
				else if(cmdBuffer[cmd_content_index-1] == 'O')
				{
					cmdState = CMD_O;
				}
				else if(cmdBuffer[cmd_content_index-1] == '\r' || cmdBuffer[cmd_content_index-1] == '\n')
				{
					cmdState = CMD_IDLE;
					uartState = UART_ERROR;
					cmd_content_index = 0;
				}
//				else if(cmdBuffer[cmd_content_index-1] != '!')
//				{
//					cmdState = CMD_IDLE;
//				}
				break;
			case CMD_R:
				if(cmdBuffer[cmd_content_index-1] == 'S')
				{
					cmdState = CMD_S;
				}
				else if(cmdBuffer[cmd_content_index-1] == '\r' || cmdBuffer[cmd_content_index-1] == '\n')
				{
					cmdState = CMD_IDLE;
					uartState = UART_ERROR;
					cmd_content_index = 0;
				}
				else
				{
					if(cmdBuffer[cmd_content_index-1] == '!')
					{
						cmdState = CMD_EXCLAMATION;
					}
					else
					{
						cmdState = CMD_IDLE;
					}
				}
				break;
			case CMD_S:
				if(cmdBuffer[cmd_content_index-1] == 'T')
				{
					cmdState = CMD_T;
				}
				else if(cmdBuffer[cmd_content_index-1] == '\r' || cmdBuffer[cmd_content_index-1] == '\n')
				{
					cmdState = CMD_IDLE;
					uartState = UART_ERROR;
					cmd_content_index = 0;
				}
				else
				{
					if(cmdBuffer[cmd_content_index-1] == '!')
					{
						cmdState = CMD_EXCLAMATION;
					}
					else
					{
						cmdState = CMD_IDLE;
					}
				}
				break;
			case CMD_T:
				if(cmdBuffer[cmd_content_index-1] == '#')
				{
					cmdState = CMD_FINISH_T;
					isRST = 1;
				}
				else if(cmdBuffer[cmd_content_index-1] == '\r' || cmdBuffer[cmd_content_index-1] == '\n')
				{
					cmdState = CMD_IDLE;
					uartState = UART_ERROR;
					cmd_content_index = 0;
				}
				else
				{
					if(cmdBuffer[cmd_content_index-1] == '!')
					{
						cmdState = CMD_EXCLAMATION;
					}
					else
					{
						cmdState = CMD_IDLE;
					}
				}
				break;
			case CMD_O:
				if(cmdBuffer[cmd_content_index-1] == 'K')
				{
					cmdState = CMD_K;
				}
				else if(cmdBuffer[cmd_content_index-1] == '\r' || cmdBuffer[cmd_content_index-1] == '\n')
				{
					cmdState = CMD_IDLE;
					uartState = UART_ERROR;
					cmd_content_index = 0;
				}
				else
				{
					if(cmdBuffer[cmd_content_index-1] == '!')
					{
						cmdState = CMD_EXCLAMATION;
					}
					else
					{
						cmdState = CMD_IDLE;
					}
				}
				break;
			case CMD_K:
				if(cmdBuffer[cmd_content_index-1] == '#')
				{
					cmdState = CMD_FINISH_K;
				}
				else if(cmdBuffer[cmd_content_index-1] == '\r' || cmdBuffer[cmd_content_index-1] == '\n')
				{
					cmdState = CMD_IDLE;
					uartState = UART_ERROR;
					cmd_content_index = 0;
				}
				else
				{
					if(cmdBuffer[cmd_content_index-1] == '!')
					{
						cmdState = CMD_EXCLAMATION;
					}
					else
					{
						cmdState = CMD_IDLE;
					}
				}
				break;
			case CMD_FINISH_K:
				if(cmdBuffer[cmd_content_index-1] == '\r' || cmdBuffer[cmd_content_index-1] == '\n')
				{
					isOK = 1;
					uartState = UART_WAIT_OK;
					cmd_content_index = 0;
					cmdState = CMD_IDLE;
				}
				break;
			case CMD_FINISH_T:
				if(cmdBuffer[cmd_content_index-1] == '\r' || cmdBuffer[cmd_content_index-1] == '\n')
				{
					uartState = UART_RST;
					cmdState = CMD_IDLE;
					cmd_content_index = 0;
				}
			default:
				break;
			}
	}
