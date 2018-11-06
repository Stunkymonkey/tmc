#!/usr/bin/env python

import csv
import json
import sys
from collections import defaultdict

points = defaultdict(list)


def read_csv(input_file):
    with open(input_file, 'r') as csvfile:
        csvreader = csv.reader(csvfile, delimiter=';')
        header = next(csvreader)
        data = []
        for row in csvreader:
            data.append(row)
    return [header], data


def write_json(output_file, data, pretty):
    with open(output_file, 'w') as outfile:
        if pretty:
            json.dump(data, outfile, indent=4)
        else:
            json.dump(data, outfile, separators=(',', ':'))


def only_collumn(index, data):
    result = []
    for row in data:
        tmp = []
        for d in index:
            tmp.append(row[d])
        result.append(tmp)
    return result


def format_points(data):
    result = []
    for row in data:
        tmp = []
        lcd = int(row[0])
        xcoord = float(int(row[1])) / 100000
        ycoord = float(int(row[2])) / 100000
        same_location = checkIfSamePos([ycoord, xcoord])
        if not same_location:
            tmp.append([lcd])
            tmp.append([ycoord, xcoord])
            result.append(tmp)
        else:
            for counter, item in enumerate(result):
                if same_location in item[0]:
                    result[counter][0].append(lcd)
                    # print("same:", same_location, item[0])
        points[lcd] = [ycoord, xcoord]
    return result


def checkIfSamePos(pos):
    for lcds, coords in points.items():
        if coords == pos:
            return lcds
    return 0


def format_offset(data):
    result = []
    store = []
    for row in data:
        lcd = int(row[0])
        n_lcd = int(row[1]) if row[1] else 0
        p_lcd = int(row[2]) if row[2] else 0
        if n_lcd != 0:
            if not CheckIfIn(store, lcd, n_lcd):
                store.append([lcd, n_lcd])
                result.append([points[lcd], points[n_lcd]])
        if p_lcd != 0:
            if not CheckIfIn(store, lcd, p_lcd):
                store.append([lcd, p_lcd])
                result.append([points[lcd], points[p_lcd]])
    return result


def CheckIfIn(data, p1, p2):
    for row in data:
        if (p1 in row) & (p2 in row):
            return True
    return False


def main():
    pretty = False
    if len(sys.argv) > 1:
        if sys.argv[1].lower() == "pretty":
            pretty = True

    p_columns = [2, 22, 23]
    p_header, p_data = read_csv("POINTS.DAT")

    p_header = only_collumn(p_columns, p_header)
    p_data = only_collumn(p_columns, p_data)
    p_data = format_points(p_data)
    write_json("points.json", p_data, pretty)

    o_columns = [2, 3, 4]
    o_header, o_data = read_csv("POFFSETS.DAT")

    o_header = only_collumn(o_columns, o_header)
    o_data = only_collumn(o_columns, o_data)
    o_data = format_offset(o_data)
    write_json("poffsets.json", o_data, pretty)


if __name__ == '__main__':
    main()
