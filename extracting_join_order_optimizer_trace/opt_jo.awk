#!/usr/bin/nawk -f
#
# (c) 2019 Nenad Noveljic All Rights Reserved
#
# usage: opt_jo [-v qb='QB_NAME'] [-v jo=join_order] optimizer_trace_file
#
# Extracting join order information from the optimizer trace
#
# see https://nenadnoveljic.com/blog/extracting-join-order-optimizer-trace
#
# version: 1.0

/Query Block/{
  qb1 = 0 ;
  if ( !qb || qb == $3 ){
    qb_line = NR " " $0 ;
    qb1 = $2 ;
    qb_printed = 0 ;
  }
}
/Join order\[/{
  jo1=0 ;
  if ( qb1 ){
    jo1=$2
    sub(/order\[/,"",jo1) ;
    sub(/\]:/,"",jo1) ;
    if ( !jo || jo1 == jo ){
      if ( !qb_printed ){
        print " " ;
        print "#################" ;
        print qb_line ;
        qb_printed = 1 ;
      }
      print "=================" ;
      print NR, $0 ;
    } else {
      jo1 = 0
    }
  }
}
/Now joining/{
  if ( jo1 ){
    print "-----------------" ;
    print NR, $0 ;
  }
}
/[A-Z][A-Z] cost:/{
  if ( jo1 ){
    print NR, $0 ;
  }
}
/Join order aborted/{
  if ( jo1 ){ 
    print NR, $0 ;
  }
}
/Best:: JoinMethod/{
  if ( jo1 ){
    print NR, $0 ;
    bjo = 1 ;
  }
}
/Cost: /{
  if (bjo){
    print NR, $0 ;
    bjo=0 ;
  }
}

