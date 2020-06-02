/*
 * (c) 2020 Nenad Noveljic All Rights Reserved
 *
 * usage: dtrace -s trace_latches.d -p PID
 *
 * trace latche allocations in an Oracle database
 *
 * Version: 1.0
 *
 * see https://nenadnoveljic.com/blog/tracing-latches-redo-copy-contention
 */

#pragma D option quiet

pid$target:oracle:kslgetl:entry
{
  printf("\n\n%s: 0X%X %d 0x%X %d\n", probefunc, arg0, arg1, arg2, arg3);
  ustack();
  printf("-----------------\n");
  start_ts[arg0] = timestamp ;
  start_vts[arg0] = vtimestamp ;
}

pid$target:oracle:kslfre:entry
/ start_ts[arg0] /
{
  this->elapsed = timestamp - start_ts[arg0] ;
  this->cpu = vtimestamp - start_vts[arg0] ;
  printf("\n\n%s: 0X%X %d 0x%X %d\n", probefunc, arg0, arg1, arg2, arg3);
  ustack();
  printf("latch elapsed[us]: %d, on cpu[us]: %d\n\n", this->elapsed/1000 , this->cpu/1000);
  printf("-----------------\n");
  start_ts[arg0] = 0 ;
  start_vts[arg0] = 0 ;
}

pid$target:oracle:kcbklbc:entry
{
  self->start_kcbklbc = timestamp ;
}

pid$target:oracle:kcbklbc:return
/ self->start_kcbklbc /
{
  printf("kcbklbc[us]: %d\n", (timestamp - self->start_kcbklbc)/1000 ) ;
  ustack();
  self->start_kcbklbc = 0 ;
}
