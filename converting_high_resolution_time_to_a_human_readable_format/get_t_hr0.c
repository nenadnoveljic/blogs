/*
 * (c) 2019 Nenad Noveljic All Rights Reserved

 * usage: get_t_hr0

 * compiling: gcc get_t_hr0.c -o get_t_hr0

 * Expressing the reference point of high-resolution time
 * in Unix Epoch time.

 * see https://nenadnoveljic.com/blog/high-resolution-time-human-readable/

 * Version: 1.0
*/

#include <time.h>
#include <sys/time.h>
#include <stdio.h>

int main()
{
  long hr_time;
  long epoch_time ;
  long diff_us ;
  struct timeval t_epoch ;
  int i;

  for ( i=0; i<10000; i++)
  {
#if defined(__sun)
    hr_time = (long)gethrtime();
#endif

#if defined(__linux__)
    struct timespec t_hr;
    clock_gettime(CLOCK_MONOTONIC, &t_hr);
    hr_time = t_hr.tv_sec * 1e9 + t_hr.tv_nsec;
#endif

    gettimeofday(&t_epoch, NULL);
  }

  epoch_time = t_epoch.tv_sec * 1e6 + t_epoch.tv_usec ;
      
  diff_us = epoch_time - hr_time/1e3 ;
  printf("%ld\n", diff_us );
}
