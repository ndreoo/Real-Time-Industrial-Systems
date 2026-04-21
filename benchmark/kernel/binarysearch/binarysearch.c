/*

  This program is part of the TACLeBench benchmark suite.
  Version V 2.0

  Name: binarysearch

  Author: Sung-Soo Lim <sslim@archi.snu.ac.kr>

  Function: binarysearch performs binary search in an array of 15 integer
    elements.
    This program is completely structured (no unconditional jumps, no exits
    from loop bodies), and does not contain switch statements, no do-while
    loops.

  Source: MRTC
          http://www.mrtc.mdh.se/projects/wcet/wcet_bench/bs/bs.c

  Original name: bs

  Changes: No major functional changes.

  License: May be used, modified, and re-distributed freely, but
           the SNU-RT Benchmark Suite must be acknowledged

*/

/*
  This program is derived from the SNU-RT Benchmark Suite for Worst
  Case Timing Analysis by Sung-Soo Lim
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
#include <errno.h>
#include <math.h>

#define PERIOD_SEC 1 // Period in seconds

unsigned long timespec_to_usec(struct timespec *ts) {
    return lround((ts->tv_sec * 1E9 + ts->tv_nsec) / 1000.0);
}

unsigned long long timespec_to_usec_ull(struct timespec *ts) {
    return llround((ts->tv_sec * 1E9 + ts->tv_nsec) / 1000.0);
}

long timespec_to_usec_long(struct timespec *ts) {
    return round((ts->tv_sec * 1E9 + ts->tv_nsec) / 1000.0);
}

void binarysearch_initSeed(void);
long binarysearch_randomInteger(void);
void binarysearch_init(void);
int binarysearch_return(void);
int binarysearch_binary_search(int);
void binarysearch_main(void);
int main(void);

volatile int binarysearch_seed;

struct binarysearch_DATA {
    int key;
    int value;
};

struct binarysearch_DATA binarysearch_data[15];
int binarysearch_result;

void binarysearch_initSeed(void) {
    binarysearch_seed = 0;
}

long binarysearch_randomInteger(void) {
    binarysearch_seed = ((binarysearch_seed * 133) + 81) % 8095;
    return (binarysearch_seed);
}

void binarysearch_init(void) {
    int i;
    binarysearch_initSeed();
    _Pragma("loopbound min 15 max 15")
    for (i = 0; i < 15; ++i) {
        binarysearch_data[i].key = binarysearch_randomInteger();
        binarysearch_data[i].value = binarysearch_randomInteger();
    }
}

int binarysearch_return(void) {
    return (binarysearch_result);
}

int binarysearch_binary_search(int x) {
    int fvalue, mid, up, low;
    low = 0;
    up = 14;
    fvalue = -1;
    _Pragma("loopbound min 1 max 4")
    while (low <= up) {
        mid = (low + up) >> 1;
        if (binarysearch_data[mid].key == x) {
            up = low - 1;
            fvalue = binarysearch_data[mid].value;
        } else if (binarysearch_data[mid].key > x) {
            up = mid - 1;
        } else {
            low = mid + 1;
        }
    }
    return (fvalue);
}

void _Pragma("entrypoint") binarysearch_main(void) {
    binarysearch_result = binarysearch_binary_search(8);
}

int main(void) {
    struct timespec req, rem;
    req.tv_sec = PERIOD_SEC;
    req.tv_nsec = 0;
    struct timespec t_now;
    long start_time, end_time;

    binarysearch_init();
    printf("binarysearch_init() called\n");


    #if CONFIG_APPBINARYSEARCH_WORKTIME
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

        	binarysearch_main();

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

       	 	binarysearch_main();

        	clock_gettime(CLOCK_REALTIME, &t_now);
        	end_time = timespec_to_usec_long(&t_now);
        	printf("bench execution time: %lld usec\n", end_time - start_time);

        	nanosleep(&req, &rem);
    	}
  #endif
}
