#! /usr/bin/python

import sys


race_results = []
errors = []

# Horse info comes from stdin as tab separated values
#   horse   lane    position    lap
for line in sys.stdin:
    line = line.replace('\n', '')
    race_results.append(line.split('\t'))

# For each line in the output file verify that there is
# not another line in the output file that occupies the same track
# position at the smae time
for index, line in enumerate(race_results):
    horse = line[0]
    lane = line[1]
    pos = line[2]
    lap = line[3]

    entry_ahead = index + 1

    while entry_ahead < len(race_results) and race_results[entry_ahead][0] != horse:
        next_horse = race_results[entry_ahead][0]
        next_lane = race_results[entry_ahead][1]
        next_pos = race_results[entry_ahead][2]
        next_lap = race_results[entry_ahead][3]
        entry_ahead += 1
        if next_lap == '1':
            continue

        if next_lane == lane and next_pos == pos:
            errors.append("Horse " + str(horse) + " conflicts with horse " + str(next_horse) + " in position (" + str(lane) + ", " + str(pos) + ")")


if len(errors) == 0:
    print("Good Job! Your horses didn't crash")
else:
    for s in errors:
        print(s)
