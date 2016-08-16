#include <stdio.h>
#include <stdlib.h>
#include "FreeRTOS.h"
#include "task.h"
#include <queue.h>
#include <math.h>



TaskHandle_t matrix_handle = NULL;
TaskHandle_t rev_handle = NULL;
QueueHandle_t rev_queue = NULL;	

struct AMessage
{
    double **c;
};

#define SIZE 10
#define ROW SIZE
#define COL SIZE
static void matrix_task() 
{
	int i;
	double **a = (double **)pvPortMalloc(ROW * sizeof(double*));
	for (i = 0; i < ROW; i++) a[i] = (double *)pvPortMalloc(COL * sizeof(double));
	double **b = (double **)pvPortMalloc(ROW * sizeof(double*));
	for (i = 0; i < ROW; i++) b[i] = (double *)pvPortMalloc(COL * sizeof(double));

    struct AMessage msg;
    msg.c = (double **)pvPortMalloc(ROW * sizeof(double*));
    for (i = 0; i < ROW; i++) msg.c[i] = (double *)pvPortMalloc(COL * sizeof(double));

	double sum = 0.0;
	int j, k, l;

	for (i = 0; i < SIZE; i++) {
		for (j = 0; j < SIZE; j++) {
			a[i][j] = 1.5;
			b[i][j] = 2.6;
		}
	}

	while (1) {
		/*
		* In an embedded systems, matrix multiplication would block the CPU for a long time
		* but since this is a PC simulator we must add one additional dummy delay.
		*/
        printf("---- Matrix task started ----\n");
        fflush(stdout);
		long simulationdelay;
		for (simulationdelay = 0; simulationdelay<1000000000; simulationdelay++)
			;
		for (i = 0; i < SIZE; i++) {
			for (j = 0; j < SIZE; j++) {
                msg.c[i][j] = 0.0;
			}
		}

		for (i = 0; i < SIZE; i++) {
			for (j = 0; j < SIZE; j++) {
				sum = 0.0;
				for (k = 0; k < SIZE; k++) {
					for (l = 0; l<10; l++) {
						sum = sum + a[i][k] * b[k][j];
					}
				}

                msg.c[i][j] = sum;
			}
		}
        if (xQueueSendToBack(rev_queue, &msg, 10) != pdPASS)
        {
            printf("Failed to post the message, even after 10 ticks.\n");
            fflush(stdout);
        }
		vTaskDelay(100);
	}
}

static void read_task()
{
    int i, j;
    struct AMessage val;
    val.c = (double **)pvPortMalloc(ROW * sizeof(double*));
    for (i = 0; i < ROW; i++) val.c[i] = (double *)pvPortMalloc(COL * sizeof(double));

    while (1)
    {
        if( xQueueReceive( rev_queue, &val, 10) )
        {
            printf("Read Task: \n");
            fflush(stdout);
            for (i = 0; i < SIZE; i++)
            {
                for (j = 0; j < SIZE; j++)
                {
                    printf("c[%d][%d] = %f\n", i, j, val.c[i][j]);
                    fflush(stdout);
                }
            }
            
        }
        vTaskDelay(100);
    }
}

int main()
{
    rev_queue = xQueueCreate(10, sizeof(struct AMessage * ));

    xTaskCreate((pdTASK_CODE)matrix_task, (signed char *)"Matrix", 1000, NULL, 1, &matrix_handle);
    xTaskCreate((pdTASK_CODE)read_task, (signed char *)"Read", 1000, NULL, 2, &rev_handle);

	/* Start running the tasks. */
	vTaskStartScheduler();
	return 0;
}

void vApplicationTickHook()
{
}


