/*

  This program is part of the TACLeBench benchmark suite.
  Version V 2.0

  Name: lift

  Author: Martin Schoeberl, Benedikt Huber

  Function: Lift Controler

  Source: C-Port from http://www.jopdesign.com/doc/jembench.pdf

  Original name: run_lift.c

  Changes: no major functional changes

  License: GPL version 3 or later

*/


/*
  Include section
*/

#ifdef __Unikraft__
#include <uk/config.h>
#endif 

#if CONFIG_TOTAL_RUNTIME_SEC
    #define TOTAL_RUNTIME_SEC CONFIG_TOTAL_RUNTIME_SEC
#endif


#include "liftlibio.h"
#include "liftlibcontrol.h"
#include <time.h>


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

void lift_controller();
void lift_init();
void lift_main();
int lift_return();


/*
  Declaration of global variables
*/

int lift_checksum;/* Checksum */


/*
  Initialization- and return-value-related functions
*/

void lift_init()
{
  unsigned int i;
  unsigned char *p;
  volatile char bitmask = 0;

  /*
    Apply volatile XOR-bitmask to entire input array.
  */
  p = ( unsigned char * ) &lift_ctrl_io_in[ 0 ];
  _Pragma( "loopbound min 40 max 40" )
  for ( i = 0; i < sizeof( lift_ctrl_io_in ); ++i, ++p )
    *p ^= bitmask;

  p = ( unsigned char * ) &lift_ctrl_io_out[ 0 ];
  _Pragma( "loopbound min 16 max 16" )
  for ( i = 0; i < sizeof( lift_ctrl_io_out ); ++i, ++p )
    *p ^= bitmask;

  p = ( unsigned char * ) &lift_ctrl_io_analog[ 0 ];
  _Pragma( "loopbound min 16 max 16" )
  for ( i = 0; i < sizeof( lift_ctrl_io_analog ); ++i, ++p )
    *p ^= bitmask;

  p = ( unsigned char * ) &lift_ctrl_io_led[ 0 ];
  _Pragma( "loopbound min 64 max 64" )
  for ( i = 0; i < sizeof( lift_ctrl_io_led ); ++i, ++p )
    *p ^= bitmask;

  lift_checksum = 0;
  lift_ctrl_init();
}


int lift_return()
{
  return ( lift_checksum - 4005888 != 0 );
}


/*
  Algorithm core functions
*/

void lift_controller()
{
  lift_ctrl_get_vals();
  lift_ctrl_loop();
  lift_ctrl_set_vals();
}


/*
  Main functions
*/

void  _Pragma( "entrypoint" ) lift_main()
{
  int i = 0;
  _Pragma( "loopbound min 1001 max 1001" )
  while ( 1 ) {
    /* zero input stimulus */
    lift_simio_in = 0;
    lift_simio_adc1 = 0;
    lift_simio_adc2 = 0;
    lift_simio_adc3 = 0;
    /* run lift_controller */
    lift_controller();
    if ( i++ >= 1000 )
      break;
  }
}


int main( void )
{

  struct timespec req, rem;
  req.tv_sec = PERIOD_SEC;
  req.tv_nsec = 0;
  struct timespec t_now;
  long start_time, end_time;

	
  lift_init();
  printf("lift_init() called\n");

  #if CONFIG_APPLIFT_WORKTIME
         struct timespec t_exec;
   	 long total_exec_start_time, total_now_exec_time;
     /*****************************************************************/
    long end_time_before_sleep = 0;
	long start_time_after_sleep=0;
	long wake_up_latency, prev_wake_up_latency = 0;
    long jitter=0;
/************************************************************************/
   	 clock_gettime(CLOCK_REALTIME, &t_exec);
   	 total_exec_start_time = timespec_to_usec_long(&t_exec);

   	 // Esegui il ciclo finché non si raggiunge il tempo massimo
    	 while ((total_now_exec_time = timespec_to_usec_long(&t_exec)) - total_exec_start_time < TOTAL_RUNTIME_SEC * 1000000) {
        	clock_gettime(CLOCK_REALTIME, &t_now);
        	start_time = timespec_to_usec_long(&t_now);
        
        	lift_main();
        
       		clock_gettime(CLOCK_REALTIME, &t_now);
        	end_time = timespec_to_usec_long(&t_now);
/********************************************************************************************************************/
            wake_up_latency = start_time - (end_time_before_sleep + 1000000);
            jitter = abs(wake_up_latency - prev_wake_up_latency);
            printf("%lld usec\t  %lld usec\t %lld usec\n", end_time - start_time, wake_up_latency, jitter);
            clock_gettime(CLOCK_REALTIME, &t_now);
            end_time_before_sleep = timespec_to_usec_long(&t_now);
            prev_wake_up_latency = wake_up_latency;
       /*****************************************************************************************************************/
        	nanosleep(&req, &rem);

        	clock_gettime(CLOCK_REALTIME, &t_exec);  // Aggiorna il tempo corrente
    	}
    
    	printf("Tempo massimo di %d secondi raggiunto, uscendo dal loop.\n", CONFIG_TOTAL_RUNTIME_SEC);

  #else
    	while (1) {
        	clock_gettime(CLOCK_REALTIME, &t_now);
        	start_time = timespec_to_usec_long(&t_now);
        
       	 	lift_main();
        
        	clock_gettime(CLOCK_REALTIME, &t_now);
        	end_time = timespec_to_usec_long(&t_now);
        	printf("bench execution time: %lld usec\n", end_time - start_time);

        	nanosleep(&req, &rem);
    	}
  #endif  

//  return ( lift_return() );
}
