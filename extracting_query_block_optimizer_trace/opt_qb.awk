#!/usr/bin/gawk -f
#
# (c) 2019 Nenad Noveljic All Rights Reserved
#
# usage: opt_qb [-v qb='qb_name'] optimizer_trace_file
#
# Extracting query block information from the optimizer trace
#
# see https://nenadnoveljic.com/blog/extracting-query-block-optimizer-trace
#
# version: 1.0

BEGIN{
  print "---------------" ;
}
/JPPD: Performing join predicate push-down/{
  jppd = $0 ;
  sub(/JPPD: Performing join predicate push-down (.+) from query block /,"",jppd) ;
  split(jppd, tmp_arr) ;
  qb_from = tmp_arr[1] ;
  qb_to = tmp_arr[6]
  jppd_from_to[qb_from "" qb_to] = NR " " $0
}
/Registered qb:/{
  if ( $5 != "(COPY" ){
    genesis_of[$3] = NR " " $0 ;
  }
}
/Final cost for query block/ {
 if ( !qb || qb == $6 ){
   l_qb = $6 ;
   print genesis_of[l_qb] ;
   for (i in jppd_from_to) {
     print jppd_from_to[i];
   }
   print NR, $0 ;
 }
}
/Best join order:/ {
  if (l_qb) {
    print NR, $0 ;
  }
}
/Cost:.*Card:/ {
  if (l_qb) {
    if (l_qb){
      print NR, $0 ;
      print "---------------" ;
     }
        l_qb = 0 ;
  }
}

