/*
 * (c) 2019 Nenad Noveljic All Rights Reserved
 *
 * usage: benchmark_get_time
 *
 * compile: gcc benchmark_timer.c -o benchmark_timer
 *
 * Measuring durations of OS kernel functions for getting 
 * wall clock and high-resolution time on Linux and Solaris.
 *
 * Version: 1.0
 *
 * see https://nenadnoveljic.com/blog/high-resolution-time-human-readable/
 */

#include<time.h>
#include <sys/time.h>
#include <stdio.h>

void start_hr_time();
void start_wallclock_time();
long get_hr_timestamp_ns();

int main()
{
  struct test {
    char* name;
    void(*function)();
  };
  struct test tests[] = 
  { 
    { "high-resolution", start_hr_time } ,
    { "wall clock     ", start_wallclock_time } 
  };

  long start, end;
  int i, j;

  printf("Duration of getting timestamps [ns]:\n");
  printf("------------------------------------\n");

  for ( i = 0; i < 10; i++ )
  {
    for ( j = 0; j < 2 ; j++ )
    {
      start = get_hr_timestamp_ns();
      (*tests[j].function)();
      end = get_hr_timestamp_ns();
      printf("%s: %ld\n", tests[j].name, end - start );
    }
    printf("\n");
  }
}

void start_hr_time()
{
#if defined(__sun)
  hrtime_t ts ;
  ts = gethrtime();
#endif

#if defined(__linux__)
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
#endif
}

void start_wallclock_time()
{
  struct timeval ts;
  gettimeofday(&ts,NULL);
}

long get_hr_timestamp_ns()
{
  long t ;

#if defined(__sun)
  t = (long)gethrtime();
#endif

#if defined(__linux__)
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  t = ts.tv_sec * 1e9 + ts.tv_nsec;
#endif

  return t;
}
