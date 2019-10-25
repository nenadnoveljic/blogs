'''
  usage: convert_tim_to_localtime.py tim hr_reference
    tim         : timestamp (in us)  from SQL trace file 
    hr_reference: high-resolution reference time converted 
                  to Epoch time, expressed in us. It can
                  be obtained by get_t_hr0.c.

  Converts the timetsamp in SQL trace in a human readable
  format.

  see https://nenadnoveljic.com/blog/high-resolution-time-human-readable/
 
  Version: 1.0
'''

import datetime
import sys

# https://stackoverflow.com/questions/21787496/converting-epoch-time-with-milliseconds-to-datetime
print (
  datetime.datetime.fromtimestamp(
      ( int (sys.argv[1]) + int (sys.argv[2]) ) / 1e6 )
    .strftime('%Y-%m-%d %H:%M:%S.%f')
)
