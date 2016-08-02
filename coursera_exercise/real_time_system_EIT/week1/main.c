#include <stdio.h>
#include <stdlib.h>
#include "FreeRTOS.h"
#include "task.h"

void Task1()
{
    portTickType xLastTime;

	xLastTime = xTaskGetTickCount();

    while (1)
    {
        printf("Task 1\n");
		vTaskDelayUntil(&xLastTime, 100/portTICK_RATE_MS);
    }
}

void Task2()
{
    portTickType xLastTime;

    xLastTime = xTaskGetTickCount();

    while (1)
    {
        printf("Task 2\n");
		vTaskDelayUntil(&xLastTime, 500/portTICK_RATE_MS);
    }
}

int main(void)
{
    xTaskCreate(Task1, (const signed char *)"Task1", 1000, NULL, 3, NULL);
    xTaskCreate(Task2, (const signed char *)"Task2", 100, NULL, 1, NULL);
	vTaskStartScheduler();

    return 0;
}
