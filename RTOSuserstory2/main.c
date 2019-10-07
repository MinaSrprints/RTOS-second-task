#include "FreeRTOS.h"
#include "task.h"
#include "led.h"
#include "LCD.h"
#include "lcd_4bit.h"
#include "keypad.h"
#include "button.h"
#include "semphr.h"
TaskHandle_t  init_handel = NULL;
TaskHandle_t A_handel = NULL;
TaskHandle_t B_handel = NULL;
TaskHandle_t C_handel = NULL;
TaskHandle_t D_handel = NULL;
SemaphoreHandle_t xSemaphore2;
SemaphoreHandle_t xSemaphore1;
SemaphoreHandle_t xSemaphoreCount;

void InitTask(void);
void B(void);
void A(void);
void C(void);
void D(void);

int main(void)
{
	xTaskCreate((TaskFunction_t)InitTask,"init",200,NULL,5,&init_handel);
	xTaskCreate((TaskFunction_t)B,"A",100,NULL,5,&B_handel);
	xTaskCreate((TaskFunction_t)A,"B",100,NULL,4,&A_handel);
	xTaskCreate((TaskFunction_t)C,"C",100,NULL,5,&C_handel);
	xTaskCreate((TaskFunction_t)D,"D",100,NULL,4,&D_handel);
	//	LED_Write(1,HIGH);
	/* Start Scheduler */
	vTaskStartScheduler();

	while(1)
	{

	}
	return 0;
}
void InitTask(void)
{
	for(;;)
	{
		LED_Init();
		BUTTON_Init(2);
		vTaskDelay(10);
		BUTTON_Init(1);
		vTaskDelay(10);
		lcd_init();
		vTaskDelay(10);
		/* Attempt to create a semaphore. */
		xSemaphore2 = xSemaphoreCreateBinary();
		xSemaphore1 = xSemaphoreCreateBinary();
		xSemaphoreCount = xSemaphoreCreateCounting((UBaseType_t)2,(UBaseType_t)0);
		vTaskDelay(10);
		vTaskSuspend(init_handel);
	}
}
void A(void)
{
	vTaskDelay(50);
	for(;;)
	{
		if(!BUTTON_GitButtonState(1))
		{
			vTaskDelay(10);
			if(!BUTTON_GitButtonState(1))
			{
				xSemaphoreGive( xSemaphore1 );
				xSemaphoreGive (xSemaphoreCount);
				//				vTaskDelay(1);
			}
		}
		vTaskDelay(10);
	}
}
void B(void)
{
	vTaskDelay(50);
	for(;;)
	{
		if(!BUTTON_GitButtonState(2))
		{
			vTaskDelay(10);
			if(!BUTTON_GitButtonState(2))
			{
				xSemaphoreGive( xSemaphore2 );
				xSemaphoreGive (xSemaphoreCount);
				//				vTaskDelay(1);
			}
		}
		vTaskDelay(10);
	}
}
void C(void)
{
	vTaskDelay(50);
	for(;;)
	{
		if (xSemaphoreTake(xSemaphoreCount,0) == pdTRUE)
		{
			LED_Write(1, HIGH);
			xSemaphoreTake(xSemaphoreCount,0);
		}
		else
		{
			LED_Write(1, LOW);
		}
		vTaskDelay(20);
	}
}
void D(void)
{
	vTaskDelay(50);
	for(;;)
	{
		if (xSemaphoreTake(xSemaphore1,0) == pdTRUE)
		{
			lcd_disp_string_xy("LED ON ",0,0);
			lcd_disp_string_xy("Button one",1,0);
		}
		else if (xSemaphoreTake(xSemaphore2,0) == pdTRUE)
		{
			lcd_disp_string_xy("LED ON ",0,0);
			lcd_disp_string_xy("Button two",1,0);
		}
		else
		{
			lcd_disp_string_xy("LED OFF",0,0);
			lcd_disp_string_xy("		          ",1,0);
		}
		vTaskDelay(100);
	}
}
