/*

  This program is part of the TACLeBench benchmark suite.
  Version V 2.0

  Name: dijkstra

  Author: unknown

  Function: dijkstra finds the shortest path between nodes in a graph

  Source: network section of MiBench

  Changes: Made some variables local, compute checksum

  License: GPL

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





#include "input.h"

/*
  Definitions of symbolic constants
*/
#define NONE 9999
#define OUT_OF_MEMORY -1
#define QUEUE_SIZE 1000

/*
  Type declarations
*/
struct _NODE {
  int dist;
  int prev;
};

struct _QITEM {
  int node;
  int dist;
  int prev;
  struct _QITEM *next;
};

/*
  Global variable definitions
*/
struct _NODE dijkstra_rgnNodes[ NUM_NODES ];

int dijkstra_queueCount;
int dijkstra_queueNext;
struct _QITEM *dijkstra_queueHead;
struct _QITEM dijkstra_queueItems[ QUEUE_SIZE ];

int dijkstra_checksum = 0;

/*
  Forward declaration of functions
*/
void dijkstra_init( void );
int dijkstra_return( void );
int dijkstra_enqueue( int node, int dist, int prev );
void dijkstra_dequeue( int *node, int *dist, int *prev );
int dijkstra_qcount( void );
int dijkstra_find( int chStart, int chEnd );
void dijkstra_main( void );
int main( void );

void dijkstra_init( void )
{
  int i, k;
  volatile int x = 0;
  _Pragma( "loopbound min 100 max 100" )
  for ( i = 0; i < NUM_NODES; i++ ) {
    _Pragma( "loopbound min 100 max 100" )
    for ( k = 0; k < NUM_NODES; k++ )
      dijkstra_AdjMatrix[ i ][ k ] ^= x;
  }

  dijkstra_queueCount = 0;
  dijkstra_queueNext = 0;
  dijkstra_queueHead = ( struct _QITEM * )0;

  dijkstra_checksum = 0;
}

int dijkstra_return( void )
{
  return ( ( dijkstra_checksum == 25 ) ? 0 : -1 );
}

int dijkstra_enqueue( int node, int dist, int prev )
{
  struct _QITEM *newItem = &dijkstra_queueItems[ dijkstra_queueNext ];
  struct _QITEM *last = dijkstra_queueHead;

  if ( ++dijkstra_queueNext >= QUEUE_SIZE )
    return OUT_OF_MEMORY;
  newItem->node = node;
  newItem->dist = dist;
  newItem->prev = prev;
  newItem->next = 0;

  if ( !last )
    dijkstra_queueHead = newItem;
  else {
    _Pragma( "loopbound min 0 max 1000" )
    while ( last->next )
      last = last->next;
    last->next = newItem;
  }
  dijkstra_queueCount++;
  return 0;
}

void dijkstra_dequeue( int *node, int *dist, int *prev )
{
  if ( dijkstra_queueHead ) {
    *node = dijkstra_queueHead->node;
    *dist = dijkstra_queueHead->dist;
    *prev = dijkstra_queueHead->prev;
    dijkstra_queueHead = dijkstra_queueHead->next;
    dijkstra_queueCount--;
  }
}

int dijkstra_qcount( void )
{
  return ( dijkstra_queueCount );
}

int dijkstra_find( int chStart, int chEnd )
{
  int ch;
  int prev, node = 0;
  int cost, dist = 0;
  int i;

  _Pragma( "loopbound min 100 max 100" )
  for ( ch = 0; ch < NUM_NODES; ch++ ) {
    dijkstra_rgnNodes[ ch ].dist = NONE;
    dijkstra_rgnNodes[ ch ].prev = NONE;
  }

  if ( chStart == chEnd ) {
  } else {
    dijkstra_rgnNodes[ chStart ].dist = 0;
    dijkstra_rgnNodes[ chStart ].prev = NONE;

    if ( dijkstra_enqueue ( chStart, 0, NONE ) == OUT_OF_MEMORY )
      return OUT_OF_MEMORY;
    
    _Pragma( "loopbound min 100 max 1000" )
    while ( dijkstra_qcount() > 0 ) {
      dijkstra_dequeue ( &node, &dist, &prev );
      _Pragma( "loopbound min 100 max 100" )
      for ( i = 0; i < NUM_NODES; i++ ) {
        if ( ( cost = dijkstra_AdjMatrix[ node ][ i ] ) != NONE ) {
          if ( ( NONE == dijkstra_rgnNodes[ i ].dist ) ||
               ( dijkstra_rgnNodes[ i ].dist > ( cost + dist ) ) ) {
            dijkstra_rgnNodes[ i ].dist = dist + cost;
            dijkstra_rgnNodes[ i ].prev = node;
            if ( dijkstra_enqueue ( i, dist + cost, node ) == OUT_OF_MEMORY )
              return OUT_OF_MEMORY;
          }
        }
      }
    }
  }
  return 0;
}

void _Pragma( "entrypoint" ) dijkstra_main( void )
{
  int i, j;

  /* finds 20 shortest paths between nodes */
  _Pragma( "loopbound min 20 max 20" )
  for ( i = 0, j = NUM_NODES / 2; i < 20; i++, j++ ) {
    j = j % NUM_NODES;
    if ( dijkstra_find( i, j ) == OUT_OF_MEMORY ) {
      dijkstra_checksum += OUT_OF_MEMORY;
      return;
    } else
      dijkstra_checksum += dijkstra_rgnNodes[ j ].dist;
    dijkstra_queueNext = 0;
  }
}

int main( void )
{


	struct timespec req, rem;
  	req.tv_sec = PERIOD_SEC;
    	req.tv_nsec = 0;
    	struct timespec t_now;
    	long start_time, end_time;


 	dijkstra_init();
	printf("dijktra_init() called\n");
	#if CONFIG_APPDIJKSTRA_WORKTIME
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

        	dijkstra_main();

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

       	 	dijkstra_main();

        	clock_gettime(CLOCK_REALTIME, &t_now);
        	end_time = timespec_to_usec_long(&t_now);
        	printf("bench execution time: %lld usec\n", end_time - start_time);

        	nanosleep(&req, &rem);
    	}
  #endif

//  return ( dijkstra_return() );
}
