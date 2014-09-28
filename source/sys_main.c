/** @file sys_main.c 
*   @brief Application main file
*   @date 9.Sep.2014
*   @version 04.01.00
*
*   This file contains an empty main function,
*   which can be used for the application.
*/

/* (c) Texas Instruments 2009-2014, All rights reserved. */

/* USER CODE BEGIN (0) */
#include "FreeRTOS.h"
#include "os_task.h"

#include "gio.h"
#include "het.h"
#include "esm.h"

volatile boolean bFast = true;

/* Define task handles. */
xTaskHandle xTask1Hadle;
xTaskHandle xTask2Hadle;

/* Task1 */
void vTaskHET(void *pvParameters)
{
	for(;;)
	{
		/* Toggle HET1[8] with timer tick. */
		gioToggleBit(hetPORT1, 8);
		vTaskDelay(200/(bFast?1:2));
	}
}

/* Task2 */
void vTaskGIO(void *pvParameters)
{
	for(;;)
	{
		/* Toggle GIOA[2] with timer tick. */
		gioToggleBit(gioPORTA, 2);
		vTaskDelay(300/(bFast?1:2));
	}
}

/* Task3 */
static void prvButtonPollTask(void *pvParameters)
{
	unsigned char ucLastState = pdFALSE, ucState;

	for (;;)
	{
		/* Check the button state. */
		ucState = gioGetBit(gioPORTA, 7);

		if (ucState != 0)
		{
			/* The button was pressed. */
			ucState = pdTRUE;
		}

		if(ucState != ucLastState)
		{
			/* The state has changed. */
			ucLastState = ucState;
			/* Toggle led speed. */
			bFast = ucState?!bFast:bFast;
		}

		vTaskDelay(10);
	}
}
/* USER CODE END */

/* Include Files */

#include "sys_common.h"

/* USER CODE BEGIN (1) */
/* USER CODE END */

/** @fn void main(void)
*   @brief Application main function
*   @note This function is empty by default.
*
*   This function is called after startup.
*   The user can use this function to implement the application.
*/

/* USER CODE BEGIN (2) */
/* USER CODE END */

void main(void)
{
/* USER CODE BEGIN (3) */
/* USER CODE END */
}

/* USER CODE BEGIN (4) */
/* USER CODE END */
