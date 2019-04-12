#
# (c) 2019 Nenad Noveljic All Rights Reserved
#
# usage: join_permutations_heatmaps.py [-h] [--text] [--qb QB] input_file
#
# Join permutations heatmap
#
# positional arguments:
# input_file      optimizer trace file
#
# optional arguments:
# -h, --help      show this help message and exit
# --text, -t      text output, no graphs
# --qb QB, -q QB  show only specified QB
#
# see https://nenadnoveljic.com/blog/join-permutations-heatmaps
#
# version: 1.0

import argparse
import re
import pandas as pd
import seaborn as sns
import numpy as np
import matplotlib.pyplot as plt

parser = argparse.ArgumentParser(description="Join permutations heatmap")
parser.add_argument('input_file', help = 'optimizer trace file')
parser.add_argument('--text', '-t', action = 'store_const', const = "text",
                    default = "graphic", help = 'text output, no graphs')
parser.add_argument('--qb', '-q', metavar = "QB",
                    help = 'show only specified QB')

args = parser.parse_args()

with open(args.input_file) as f:
  content = f.readlines()

reg_join_order = re.compile(
  'Join order\[(?P<join_order_number>\d+)\]:\s*(?P<join_order_text>.+)')

reg_qb = re.compile('.* QB\s+has\s+object\s+(?P<qb>\S+)')

reg_best_jo = re.compile('.*Best join order\:.*')

filter_qb = args.qb

df = None
qb = None

for line in content:
  matched_join_order = reg_join_order.match(line)
  matched_qb = reg_qb.match(line)
  matched_best_jo = reg_best_jo.match(line)

  if matched_qb:
    qb = matched_qb.group('qb')

  elif matched_best_jo and ( filter_qb is None or qb == filter_qb ) :
    print ("     ")
    print ("---------------")
    if qb is not None:
      print ("query block: " + qb) ;

    if args.text == "text":
      print(df.to_string())
    else:
      index = df.index.tolist()
      index =  np.core.defchararray.replace( index, '$' , '\$')
      df.index = index

      plt.switch_backend('QT4Agg')
      plt.get_backend()

      sns.heatmap(
        df, annot = True, fmt = "d", linewidths=2 , cmap="YlGnBu"
      )

      plt.subplots_adjust(left=0.25)
      locs, labels = plt.xticks()
      plt.setp(labels, rotation=0)
      plt.xlabel('join position')
      plt.ylabel('tables/query blocks')

      mng = plt.get_current_fig_manager()
      mng.window.showMaximized()

      if qb is not None:
        plt.title(qb)

      plt.show()

  elif matched_join_order:

    join_order_number = matched_join_order.group('join_order_number')
    join_order_text = matched_join_order.group('join_order_text')

    join_order_text = re.sub('#\d+','',join_order_text)
    join_order_text = join_order_text.split()

    if join_order_number == "1":

      length = len(join_order_text)
      cols = list ( range(1,length + 1) )

      df = pd.DataFrame(
        np.zeros(( length , length ), dtype=int) ,index = join_order_text ,
        columns =  cols
      )

    i = 1
    for table in join_order_text:
      df.at[table,i] += 1
      i += 1

    join_order_number = None
    join_order_text = None
