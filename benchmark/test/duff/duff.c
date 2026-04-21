/*

  This program is part of the TACLeBench benchmark suite.
  Version V 2.0

  Name: duff

  Author: Jakob Engblom

  Function: Duff's device

  Source: MRTC
          http://www.mrtc.mdh.se/projects/wcet/wcet_bench/duff/duff.c

  Changes: no major functional changes

  License: may be used, modified, and re-distributed freely

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
unsigned long
timespec_to_usec(struct timespec *ts)
{
        return lround((ts->tv_sec * 1E9 + ts->tv_nsec) / 1000.0);
}

unsigned long long
timespec_to_usec_ull(struct timespec *ts)
{
        return llround((ts->tv_sec * 1E9 + ts->tv_nsec) / 1000.0);
}

long
timespec_to_usec_long(struct timespec *ts)
{
        return round((ts->tv_sec * 1E9 + ts->tv_nsec) / 1000.0);
}



/*
  Forward declaration of functions
*/

void duff_copy( char *to, char *from, int count );
void duff_initialize( char *arr, int length );
void duff_init();
void duff_main( void );
int duff_return( void );
int main( void );


/*
  Declaration of global variables
*/

char duff_source[ 100 ];
char duff_target[ 100 ];


/*
  Initialization- and return-value-related functions
*/

void duff_init()
{
  unsigned int i;
  unsigned char *p;
  volatile char bitmask = 0;

  duff_initialize( duff_source, 100 );

  /*
    Apply volatile XOR-bitmask to entire input array.
  */
  p = ( unsigned char * ) &duff_source[  0  ];
  _Pragma( "loopbound min 400 max 400" )
  for ( i = 0; i < sizeof( duff_source ); ++i, ++p )
    *p ^= bitmask;
}


int duff_return( void )
{
  return ( duff_target[ 28 ] - 72 != 0 );
}


/*
  Algorithm core functions
*/

void duff_initialize( char *arr, int length )
{
  int i;

  _Pragma( "loopbound min 100 max 100" )
  for ( i = 0; i < length; i++ )
    arr[ i ] = length - i;
}


void duff_copy( char *to, char *from, int count )
{
  int n = ( count + 7 ) / 8;

  _Pragma( "marker outside" )
  switch ( count % 8 ) {
    case 0:
      do {
        *to++ = *from++;
      case 7:
        *to++ = *from++;
      case 6:
        *to++ = *from++;
      case 5:
        *to++ = *from++;
      case 4:
        *to++ = *from++;
      case 3:
        *to++ = *from++;
      case 2:
        *to++ = *from++;
      case 1:
        _Pragma( "marker inside" )
        *to++ = *from++;


      } while ( --n > 0 );
  }
  _Pragma( "flowrestriction 1*inside <= 6*outside" )
}


/*
  Main functions
*/

void _Pragma( "entrypoint" ) duff_main( void )
{
  duff_copy( duff_target, duff_source, 43 );
}


int main( void )
{

	struct timespec req, rem;
  	req.tv_sec = PERIOD_SEC;
    	req.tv_nsec = 0;
    	struct timespec t_now;
    	long start_time, end_time;

	duff_init();
	printf("duff_init() called! \n");  
	
	#if CONFIG_APPDUFF_WORKTIME
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

        	duff_main();

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

       	 	duff_main();

        	clock_gettime(CLOCK_REALTIME, &t_now);
        	end_time = timespec_to_usec_long(&t_now);
        	printf("bench execution time: %lld usec\n", end_time - start_time);

        	nanosleep(&req, &rem);
    	}
  #endif

//  return ( duff_return() );
}

