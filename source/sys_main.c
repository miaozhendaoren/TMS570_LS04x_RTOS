/** @file sys_main.c 
*   @brief Application main file
*   @date 9.Sep.2014
*   @version 00.00.02
*/

/* Include Files */

#include "sys_common.h"
#include "system.h"

/* USER CODE BEGIN */
#include "FreeRTOS.h"
#include "os_task.h"

#include "adc.h"
#include "esm.h"
#include "gio.h"
#include "het.h"
#include "sci.h"

/* Define task handles. */
xTaskHandle xTask1Hadle;

/* Task1 */
void vTaskADC(void *pvParameters)
{
	#define  TSIZE1 12
	uint8  TEXT1[TSIZE1]= {'\r','\n','|','\t','C','H','.','I','D','=','0','x'};
	#define  TSIZE2 9
	uint8  TEXT2[TSIZE2]= {'\t','V','A','L','U','E','=','0','x'};

	adcData_t adc_data[2];
	void sciDisplayText(sciBASE_t *sci, uint8 *text, uint32 length);
	void sciDisplayData(sciBASE_t *sci, uint8 *text, uint32 length);

    uint32 ch_count = 0;
    uint32 id = 0;
    uint32 value = 0;

    for (;;)
    {
        /* start adc conversion */
        adcStartConversion(adcREG1, adcGROUP1);

        /* ... wait and read the conversion count */
        while((adcIsConversionComplete(adcREG1, adcGROUP1)) == 0);
        ch_count = adcGetData(adcREG1, adcGROUP1, &adc_data[0]);

        ch_count = ch_count;
        /* conversion results :                                       */
        /* adc_data[0] -> should have conversions for Group1 channel1 */
        /* adc_data[1] -> should have conversions for Group1 channel2 */

        id = adc_data[0].id;
        value = adc_data[0].value;

        sciDisplayText(scilinREG, &TEXT1[0], TSIZE1); /* send text 1 */
        sciDisplayData(scilinREG, (uint8*)&id, 4);    /* send data 1 */
        sciDisplayText(scilinREG, &TEXT2[0], TSIZE2); /* send text 2 */
        sciDisplayData(scilinREG, (uint8*)&value, 4); /* send data 2 */

        id = adc_data[1].id;
        value = adc_data[1].value;

        sciDisplayText(scilinREG, &TEXT1[0], TSIZE1); /* send text 1 */
        sciDisplayData(scilinREG, (uint8*)&id, 4);    /* send data 1 */
        sciDisplayText(scilinREG, &TEXT2[0], TSIZE2); /* send text 2 */
        sciDisplayData(scilinREG, (uint8*)&value, 4); /* send data 2 */

        vTaskDelay(1000);
    }
}

/* Functions */
void sciDisplayData(sciBASE_t *sci, uint8 *text, uint32 length)
{
    uint8 txt = 0;
    uint8 txt1 = 0;

#if ((__little_endian__ == 1) || (__LITTLE_ENDIAN__ == 1))
text = text + (length-1);
#endif

    while(length--)
    {
#if ((__little_endian__ == 1) || (__LITTLE_ENDIAN__ == 1))
        txt = *text--;
#else
        txt = *text++;
#endif

        txt1 = txt;

        txt  &= ~(0xF0);
        txt1 &= ~(0x0F);
        txt1 = txt1 >> 4;

        if(txt <= 0x9)
        {
            txt += 0x30;
        }
        else if(txt > 0x9 && txt < 0xF)
        {
            txt += 0x37;
        }
        else
        {
            txt = 0x30;
        }

        if(txt1 <= 0x9)
        {
            txt1 += 0x30;
        }
        else if((txt1 > 0x9) && (txt1 <= 0xF))
        {
            txt1 += 0x37;
        }
        else
        {
            txt1 = 0x30;
        }

        while ((scilinREG->FLR & 0x4) == 4); /* wait until busy */
        sciSendByte(scilinREG, txt1);        /* send out text   */
        while ((scilinREG->FLR & 0x4) == 4); /* wait until busy */
        sciSendByte(scilinREG, txt);         /* send out text   */
    };
}

void sciDisplayText(sciBASE_t *sci, uint8 *text, uint32 length)
{
    while(length--)
    {
        while ((scilinREG->FLR & 0x4) == 4); /* wait until busy */
        sciSendByte(scilinREG, *text++);     /* send out text   */
    };
}
/* USER CODE END */

/** @fn void main(void)
*   @brief Application main function
*/

void main(void)
{
	adcInit();
	gioInit();
	hetInit();
	sciInit();

	if (xTaskCreate(vTaskADC, "TaskADC", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, &xTask1Hadle) != pdTRUE)
	{
		while(1);
	}

	vTaskStartScheduler();

	while(1);
}
