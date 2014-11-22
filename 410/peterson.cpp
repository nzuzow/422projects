using namespace std;

#include <unistd.h>     /* Symbolic Constants */
#include <sys/types.h>  /* Primitive System Data Types */ 
#include <errno.h>      /* Errors */
#include <stdio.h>      /* Input/Output */
#include <stdlib.h>     /* General Utilities */
#include <pthread.h>    /* POSIX Threads */
#include <string.h>     /* String handling */
#include <iostream>
#include "cs.cpp"

int flag[6];
int turn, turn2, turn3;
int pet3, pet2;

#define randomdelay usleep(rand()%20)


void *peterson(void * ptr)
{
  int *temp = (int *) ptr;
  int i; 
  int j;
  if (i == 0 || i == 1) {
	i = 4;
  	j = 5;
  }
  else if (i == 2 || i == 3) {
	i = 5;
	j = 4;
  }
  else cout << "error" << endl;

  while(1)
    {
	cout << "in final battle while loop" << endl;
      // Non-critical section
      // We do not really care what happens here. But we care that it may take different times. Hence, will invoke a random number generator for doing wait
      
      randomdelay;
	cout << "setting flag i to 1" << endl;
      flag[i] = 1;
	cout << "set flag i to 1" << endl;
      // There is a possibility that the thread may get swapped out here. So, add a random delay
      randomdelay;
      turn = j;
      randomdelay;
      while(flag[j]==1 && turn==j);
      // Entering CS
      cout << "in critical section" << endl;
      if (i == 4) i = pet3;
      if (i == 5) i = pet2;
      cs(i);
      flag[0] = 0;
      flag[1] = 0;
      flag[2] = 0;
      flag[3] = 0;
      flag[4] = 0;
      flag[5] = 0;
      break;
      sleep(10);
    }
}

void *peterson2(void * ptr2)
{
  int *temp2 = (int *) ptr2;
  int k = *temp2;
  int l;
  if (k == 2)
  	int l = 3;
  if (k == 3)
	int l = 2;

  while(1)
    {
      // Non-critical section
      // We do not really care what happens here. But we care that it may take different times.
      // Hence, will invoke a random number generator for doing wait

      randomdelay;
      flag[k] = 1;
      // There is a possibility that the thread may get swapped out here. So, add a random delay
      randomdelay;
      turn2 = l;
      randomdelay;
      while(flag[l]==1 && turn2==l);
      pet2 = k;
      peterson(ptr2);
      flag[k] = 0;
      // Entering CS
    }
}

void *peterson3(void * ptr3)
{
  int *temp3 = (int *) ptr3;
  int m = *temp3;
  cout << "m: " << m << endl;
  int n;
  if (m == 0)
  	int n = 1;
  if (m == 1)
 	int n = 0;

  while(1)
    {
      // Non-critical section
      // We do not really care what happens here. But we care that it may take different times.
      // Hence, will invoke a random number generator for doing wait

      randomdelay;
      flag[m] = 1;
      // There is a possibility that the thread may get swapped out here. So, add a random delay
      randomdelay;
      turn3 = n;
      randomdelay;
      while(flag[n]==1 && turn3==n);
      pet3 = m;
      peterson(ptr3);
      flag[m] = 0;
      // Entering CS
    }
}


int main()
{
  flag[0] = 0;
  flag[1] = 0;
  flag[2] = 0;
  flag[3] = 0;
  flag[4] = 0;
  flag[5] = 0;
  turn = 0; // Not needed since turn is never used without being set first
  turn2 = 0;
  turn3 = 0;
     
  pthread_t thread1, thread2, thread3, thread4;  /* thread variables */
  int p0 = 0;
  int p1 = 1;
  int p2 = 2;
  int p3 = 3;

  pthread_create(&thread1, NULL,  peterson3, (void *) &p0);
  pthread_create(&thread2, NULL,   peterson3, (void *) &p1);
  pthread_create(&thread3, NULL,  peterson2, (void *) &p2);
  pthread_create(&thread4, NULL,  peterson2, (void *) &p3);


  pthread_join(thread1, NULL);
  pthread_join(thread2, NULL);
  pthread_join(thread3, NULL);
  pthread_join(thread4, NULL);
              
  /* exit */  
  exit(0);
} 
