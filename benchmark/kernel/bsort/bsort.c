/*

  This program is part of the TACLeBench benchmark suite.
  Version 2.0

  Name: bsort

  Author: unknown

  Function: A program for testing the basic loop constructs,
            integer comparisons, and simple array handling by
            sorting 100 integers

  Source: MRTC
          http://www.mrtc.mdh.se/projects/wcet/wcet_bench/bsort100/bsort100.c

  Original name: bsort100

  Changes: See ChangeLog.txt

  License: May be used, modified, and re-distributed freely.

*/


/*
  Forward declaration of functions
*/
#ifdef __Unikraft__
#include <uk/config.h>
#endif

#if CONFIG_TOTAL_RUNTIME_SEC
    #define TOTAL_RUNTIME_SEC CONFIG_TOTAL_RUNTIME_SEC
#endif





#include <stdio.h>
#include <time.h>
#include <math.h>

#define PERIOD_SEC 1 // Periodo in secondi

unsigned long timespec_to_usec(struct timespec *ts)
{
    return lround((ts->tv_sec * 1E9 + ts->tv_nsec) / 1000.0);
}

long timespec_to_usec_long(struct timespec *ts)
{
    return round((ts->tv_sec * 1E9 + ts->tv_nsec) / 1000.0);
}

void bsort_init(void);
void bsort_main(void);
int bsort_return(void);
int bsort_Initialize(int Array[]);
int bsort_BubbleSort(int Array[]);

#define bsort_SIZE 100

static int bsort_Array[bsort_SIZE];

int bsort_Initialize(int Array[])
{
    int Index;
    _Pragma("loopbound min 100 max 100")
    for (Index = 0; Index < bsort_SIZE; Index++)
        Array[Index] = (Index + 1) * -1;

    return 0;
}

void bsort_init(void)
{
    bsort_Initialize(bsort_Array);
}

int bsort_return(void)
{
    int Sorted = 1;
    int Index;
    _Pragma("loopbound min 99 max 99")
    for (Index = 0; Index < bsort_SIZE - 1; Index++)
        Sorted = Sorted && (bsort_Array[Index] < bsort_Array[Index + 1]);

    return 1 - Sorted;
}

int bsort_BubbleSort(int Array[])
{
    int Sorted = 0;
    int Temp, Index, i;

    _Pragma("loopbound min 99 max 99")
    for (i = 0; i < bsort_SIZE - 1; i++) {
        Sorted = 1;
        _Pragma("loopbound min 3 max 99")
        for (Index = 0; Index < bsort_SIZE - 1; Index++) {
            if (Index > bsort_SIZE - i)
                break;
            if (Array[Index] > Array[Index + 1]) {
                Temp = Array[Index];
                Array[Index] = Array[Index + 1];
                Array[Index + 1] = Temp;
                Sorted = 0;
            }
        }
        if (Sorted)
            break;
    }
    return 0;
}

void _Pragma("entrypoint") bsort_main(void)
{
    bsort_BubbleSort(bsort_Array);
}

int main(void)
{
    struct timespec req, rem;
    req.tv_sec = PERIOD_SEC;
    req.tv_nsec = 0;
    struct timespec t_now;
    long start_time, end_time;

    bsort_init();
    printf("bsort_init() called\n");
      #if CONFIG_APPBSORT_WORKTIME
         struct timespec t_exec;
        long total_exec_start_time, total_now_exec_time;

        long end_time_before_sleep = 0;
        long start_time_after_sleep=0;
        long wake_up_latency, prev_wake_up_latency = 0;
        long jitter=0;

        clock_gettime(CLOCK_REALTIME, &t_exec);
        total_exec_start_time = timespec_to_usec_long(&t_exec);

        // Esegui il ciclo finché non si raggiunge il tempo massimo
    	 while ((total_now_exec_time = timespec_to_usec_long(&t_exec)) - total_exec_start_time < TOTAL_RUNTIME_SEC * 1000000) {
        	clock_gettime(CLOCK_REALTIME, &t_now);
        	start_time = timespec_to_usec_long(&t_now);

        	bsort_main();

       		clock_gettime(CLOCK_REALTIME, &t_now);
        	end_time = timespec_to_usec_long(&t_now);
        	  wake_up_latency = start_time - (end_time_before_sleep + 1000000);
            jitter = abs(wake_up_latency - prev_wake_up_latency);
            printf("%lld usec\t  %lld usec\t %lld usec\n", end_time - start_time, wake_up_latency, jitter);
            clock_gettime(CLOCK_REALTIME, &t_now);
            end_time_before_sleep = timespec_to_usec_long(&t_now);
            prev_wake_up_latency = wake_up_latency;

        	nanosleep(&req, &rem);

        	clock_gettime(CLOCK_REALTIME, &t_exec);  // Aggiorna il tempo corrente
    	}

    	printf("Tempo massimo di %d secondi raggiunto, uscendo dal loop.\n", CONFIG_TOTAL_RUNTIME_SEC);

  #else
    	while (1) {
        	clock_gettime(CLOCK_REALTIME, &t_now);
        	start_time = timespec_to_usec_long(&t_now);

       	 	bsort_main();

        	clock_gettime(CLOCK_REALTIME, &t_now);
        	end_time = timespec_to_usec_long(&t_now);
        	printf("bench execution time: %lld usec\n", end_time - start_time);

        	nanosleep(&req, &rem);
    	}
  #endif
}
