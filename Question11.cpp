
#include  <stdio.h>
#include  <stdlib.h>
#include  <time.h>
#include  <thread.h>
#include  "bridge-m.h"


#define   MAX_CROSSING  5
#define   MAX_THREADS   20

mutex_t   Screen;
mutex_t   RandomNumber;

int       Max_Run;

void  *OneVehicle(void *voidPTR)
{
     int     *intPTR = (int *) voidPTR;
     int     ID = *intPTR;
     int     Direction;
     char    *Dir[2] = { "north", "south" };
     char    space[200];
     int     i;
     double  D;

     mutex_lock(&Screen);
          printf("%sVehicle %d started ...\n", space, ID);
     mutex_unlock(&Screen);
     for (i = 1; i <= Max_Run; i++) {   /* for each crossing   */
          thr_yield();                  /* rest for a while         */
          mutex_lock(&RandomNumber);    /* lock random # generator  */
               D = rand() / BIG;        /* generate a random number */
          mutex_unlock(&RandomNumber);  /* release random # gen.    */
          Direction = (D <= 0.5) ? 0 : 1;    /* which direction?    */
          mutex_lock(&Screen);
               printf("%sVehicle %d (%d) arrives at the bridge in "
                      "direction %s\n", space, ID, i, Dir[Direction]);
          mutex_unlock(&Screen);
          ArriveBridge(Direction);      /* arrive at the bridge     */
          mutex_lock(&Screen);
               printf("%sVehicle %d (%d) crosses the bridge\n", space, ID, i);
          mutex_unlock(&Screen);
          thr_yield();                  /* crossing the bridge      */
          ExitBridge(Direction);        /* exit the bridge          */
          mutex_lock(&Screen);
               printf("%sVehicle %d (%d) is done\n", space, ID, i);
          mutex_unlock(&Screen);
     }
     mutex_lock(&Screen);
          printf("%sVehicle %d is gone forever ...\n", space, ID);
     mutex_unlock(&Screen);
     thr_exit(0);
}

/* ----------------------------------------------------------------- */
/*                        The main program                           */
/* ----------------------------------------------------------------- */

void  main(int argc, char *argv[])
{
     thread_t   ID[MAX_THREADS];         /* vehicle ID               */
     size_t     Status[MAX_THREADS];     /* vehicle status           */
     int        Arg[MAX_THREADS];        /* vehicle argument         */
     int        Threads;                 /* # of vehicles            */
     int        i;

     if (argc != 3)  {
          printf("Use %s #-of-iterations #-of-vehicles\n", argv[0]);
          exit(0);
     }
     Max_Run = abs(atoi(argv[1]));
     Threads = abs(atoi(argv[2]));
     if (Threads > MAX_THREADS) {
          printf("The no. of vehicles is too large.  Reset to %d\n",
                 MAX_THREADS);
          Threads = MAX_THREADS;
     }

     printf(" started ...\n");

     mutex_init(&Screen, USYNC_THREAD, (void *) NULL);
     BridgeInit();
     srand((unsigned) time(NULL));

     for (i = 0; i < Threads; i++) {     /* start vehicles          */
          Arg[i] = i+1;
          thr_create(NULL, 0, OneVehicle, (void *) &(Arg[i]),
                     0, (void *) &(ID[i]));
     }
     for (i = 0; i < Threads; i++)       /* wait for vehicles       */
          thr_join(ID[i], 0, (void *) &(Status[i]));

     printf(" exits ...\n");
}


