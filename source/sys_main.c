/** @file sys_main.c 
*   @brief Application main file
*   @date 9.Sep.2014
*   @version 00.00.01
*/

/* Include Files */

#include "sys_common.h"
#include "system.h"

/* USER CODE BEGIN */
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

/** @fn void main(void)
*   @brief Application main function
*/

void main(void)
{
	gioInit();
	hetInit();

	/* Create Task1 */
	if (xTaskCreate(vTaskHET, "TaskHET", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, &xTask1Hadle) != pdTRUE)
	{
		while(1);
	}

	/* Create Task2 */
	if (xTaskCreate(vTaskGIO, "TaskGIO", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, &xTask2Hadle) != pdTRUE)
	{
		while(1);
	}

	xTaskCreate(prvButtonPollTask, "TaskButton", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL);

	vTaskStartScheduler();

	while(1);
}
