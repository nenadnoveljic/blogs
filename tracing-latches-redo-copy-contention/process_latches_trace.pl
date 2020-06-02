#!/bin/perl

# (c) 2019 Nenad Noveljic All Rights Reserved
# 
# usage: process_latches_trace.pl trace_file mapping_file
#   trace_file is produced by trace_latches.dbmclose
#   mapping_file contains latch addresses in the format: addr latch name
#
# translates latch addresses in the latch trace to the latch name
#
# Version: 1.0
# 
# see https://nenadnoveljic.com/blog/tracing-latches-redo-copy-contention

use strict ;
use warnings ;

my $input_file = $ARGV[0];
my $mapping_file = $ARGV[1];

open my $mapping_fh, '<', $mapping_file;
my %latch_on;
while ( my $line = <$mapping_fh> ) {
  chomp $line;
  $line =~ s{\s*$}{};
  if ( $line =~ m{(0X\w+)\s+(.*)}xms ){
    my $addr = $1;
    my $latch = $2;
    $latch_on{$addr} = $latch;
  }
}
close $mapping_fh;

open my $input_fh, '<', $input_file;
while ( my $line = <$input_fh> ) {
  if ( $line =~ m{(kslgetl|kslfre):\s+(\w+)(.*)}xms ){
    my $latch = exists $latch_on{$2} ? $latch_on{$2} : 'UNKNOWN';
    $line = "$1: $latch $2$3";
  }
  print $line;
}
