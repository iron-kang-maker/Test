#include <stdio.h>
#include <stdlib.h>
#include "FreeRTOS.h"
#include "task.h"
#include <queue.h>
#include <timers.h>
#include <math.h>



TaskHandle_t matrix_handle = NULL;
TaskHandle_t aperiodic_handle = NULL;
TimerHandle_t xTimers;
int count = 0;
int time = 0;

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
        printf("%d: ---- Matrix task started ----\n", time);
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
        printf("%d: ---- Matrix task Done ----\n", time);
		vTaskDelay(100);
	}
}

static void aperiodic_task()
{
    count = 0;
    printf("%d: ==== Aperiodic task started! ====\n", time);
    fflush(stdout);
	long i;
	for (i = 0; i<1000000000; i++); //Dummy workload   
    printf("%d: ==== Aperiodic task done! total time:(%d) ====\n", time, count);
	fflush(stdout);
    aperiodic_handle = NULL;
	vTaskDelete(aperiodic_handle);
    
}

/* A variable to hold a count of the number of times the timer expires. */
long lExpireCounters = 0;
static void vTimerCallback(xTimerHandle pxTimer)
{
    printf("%d: Timer callback!\n", time);
    if (aperiodic_handle == NULL)
	    xTaskCreate((pdTASK_CODE)aperiodic_task, (signed char *)"Aperiodic", configMINIMAL_STACK_SIZE, NULL, 4, &aperiodic_handle);
	long lArrayIndex;
	const long xMaxExpiryCountBeforeStopping = 10;
	/* Optionally do something if the pxTimer parameter is NULL. */
	configASSERT(pxTimer);
	/* Increment the number of times that pxTimer has expired. */
	lExpireCounters += 1;
	/* If the timer has expired 10 times then stop it from running. */
	if (lExpireCounters == xMaxExpiryCountBeforeStopping) {
		/* Do not use a block time if calling a timer API function from a
		timer callback function, as doing so could cause a deadlock! */
		xTimerStop(pxTimer, 0);
	}
}

int main()
{

    xTimers = xTimerCreate((signed char *)"Timer",
                           5000/portTICK_RATE_MS,
                           pdTRUE,
                           ( void * ) 0,
                           vTimerCallback);

    xTimerStart(xTimers, 0);
    xTaskCreate((pdTASK_CODE)matrix_task, 
                (signed char *)"Matrix", 
                 1000, NULL, 
                 3, 
                 &matrix_handle);

	/* Start running the tasks. */
	vTaskStartScheduler();
	return 0;
}

void vApplicationTickHook()
{
    count++;
    time++;
}


