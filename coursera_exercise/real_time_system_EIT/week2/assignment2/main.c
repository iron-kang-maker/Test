#include <stdio.h>
#include <stdlib.h>
#include "FreeRTOS.h"
#include "task.h"

#define SIZE 10
#define ROW SIZE
#define COL SIZE

static void  prvInitialiseHeap(void);

void vApplicationMallocFailedHook(void);
void vApplicationIdleHook(void);
void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName);
void vApplicationTickHook(void);

static void prvSaveTraceFile(void);
traceLabel xTickTraceUserEvent;
static portBASE_TYPE xTraceRunning = pdTRUE;

TaskHandle_t matrix_handle = NULL;
TaskHandle_t communication_handle = NULL;
int task = NONE;
int count = 0;
int count_matrix = 0;
int total_count = 0;
int total_count_matrix = 0;

static void matrix_task() 
{
	int i;
	double **a = (double **)pvPortMalloc(ROW * sizeof(double*));
	for (i = 0; i < ROW; i++) a[i] = (double *)pvPortMalloc(COL * sizeof(double));
	double **b = (double **)pvPortMalloc(ROW * sizeof(double*));
	for (i = 0; i < ROW; i++) b[i] = (double *)pvPortMalloc(COL * sizeof(double));
	double **c = (double **)pvPortMalloc(ROW * sizeof(double*));
	for (i = 0; i < ROW; i++) c[i] = (double *)pvPortMalloc(COL * sizeof(double));

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
        count_matrix = 0;
        printf("Start Matrix task ---->\n");
        fflush(stdout);
		long simulationdelay;
		for (simulationdelay = 0; simulationdelay<1000000000; simulationdelay++)
			;
		for (i = 0; i < SIZE; i++) {
			for (j = 0; j < SIZE; j++) {
				c[i][j] = 0.0;
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
				c[i][j] = sum;
			}
		}
        printf("End Matrix task: time(%d)\n", count_matrix);
        fflush(stdout);
		vTaskDelay(100);
        printf("Delay Matrix task: time(%d)\n", count_matrix);
        fflush(stdout);
	}
}

static void communication_task()
{
	while (1) {
        count = 0;
		printf("Sending data...\n");
        fflush(stdout);

		vTaskDelay(100);
		fio_printf("Data sent!\n");
        fflush(stdout);

		vTaskDelay(100);
        total_count = count;
        printf("Data sent finish: time(%d)\n", total_count);
        fflush(stdout);
	}
}

static void priorityset_task()
{
    int old_total_count = 0;
    while (1)
    {
        if (old_total_count != total_count)
        {
            if (total_count > 1000)
            {
                vTaskPrioritySet(communication_handle, 4);
                printf("priority task: Change priority to 4\n");
                fflush(stdout);
            }
            else if (total_count < 200)
            {
                vTaskPrioritySet(communication_handle, 2);
                printf("priority task: Change priority to 2\n");
                fflush(stdout);
            }
            old_total_count = total_count;
        }
        else
        {
            printf("priority task!\n");
            fflush(stdout);
        }
        vTaskDelay(50);
    }
}

int main()
{
    BaseType_t xReturned1, xReturned2, xReturned3;

    prvInitialiseHeap();
	vTraceInitTraceData();
	xTickTraceUserEvent = xTraceOpenLabel( "tick" );
	

    xReturned1 = xTaskCreate(
                 (pdTASK_CODE)matrix_task, 
                 (signed char *)"Matrix", 
                 1000, 
                 NULL, 
                    3, 
                 &matrix_handle);

    xReturned2 = xTaskCreate(
                 (pdTASK_CODE)communication_task, 
                 (signed char *)"Communication",
                 configMINIMAL_STACK_SIZE, 
                 NULL, 
                    1, 
                 &communication_handle);

    xReturned3 = xTaskCreate(
                 (pdTASK_CODE)priorityset_task, 
                 (signed char *)"PrioritySet", 
                 configMINIMAL_STACK_SIZE, 
                 NULL, 
                    0, 
                 NULL);

	vTaskStartScheduler();

	return 0;
}

void vApplicationTickHook()
{
    count++;
    count_matrix++;
}

