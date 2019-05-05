#!/bin/sh

#  start.sh
#  workspace
#
#  Created by Emmanuel Utomi on 5/5/19.
#  Copyright © 2019 Uche. All rights reserved.
cd proj6
make all
./derby > results.txt

cat results.txt | awk '$1=="Horse:" {print $2"\t"$4"\t"$6"\t"$8}' | sed 's\,\\g' | ./clash_detection.py
