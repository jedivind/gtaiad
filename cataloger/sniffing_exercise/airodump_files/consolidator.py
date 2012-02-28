import argparse
import csv
import sys
import os
import io
from pprint import pprint
import pdb

SUMMARY_BSSID = 0
SUMMARY_ESSID = 13

BEACONS_BSSID = 0
BEACONS_ESSID = 6
BEACONS_ESSID_LENGTH = 5

# This file works with Python 2.7 and Python 3

USAGE_EPILOG = """\
Examples:

  Consolidate all .csv and .gatech.csv files in 0226-PK/02261620-PK-2K006:

    python consolidator.py --new > some_data.csv
    python consolidator.py --path 0226-PK/02261620-PK-2K006 >> some_data.csv

  Consolidate all .csv and .gatech.csv files in 0226-PK:

    python consolidator.py --new > all_data.csv
    for a in `find 0226-PK/* -type d`; do python consolidator.py --path $a; done >> all_data.csv
"""

def parse_args(argv):
  "Parse arguments on the command line"

  parser = argparse.ArgumentParser(
      description = 'Consolidate airocrack-ng files\n\n' + USAGE_EPILOG,
      formatter_class=argparse.RawDescriptionHelpFormatter)

  # If --new is provided, nothing else is used
  parser.add_argument('--new', action='store_true',
      help='create headers for a new consolidation file')

  parser.add_argument('--path',
      help='path to airodump-ng GATECH_CSV and CSV files')

  return parser.parse_args()


def output_headers():
  "One-time use function to create the new CSV field names."

  print('capture_user,capture_location,BSSID,time,power,channel,sequence,essid_length,ESSID')


def rewrite_input_files(path):
  """\
  Locate the .csv and gatech.csv files in `path`.  Add the user who recorded
  the data and the location tag.
  """

  all_files = os.listdir(path)
  if len(all_files) > 2:
    print("Too many files found.  Feature required.")
    sys.exit(1)

  path_parts = os.path.basename(all_files[0]).split('-')
  capture_user = path_parts[0]
  capture_location = path_parts[1]

  for filename in all_files:
    if filename.endswith('.gatech.csv'):
      # Read the file into a list.  Strip out any non-ASCII characters since
      # the CSV parser can't handle those.
      infile = open(os.path.join(path, filename), 'rb')
      inlines = []

      for line in infile:
        new_line = ''.join([chr(ch) for ch in line if chr(ch).isprintable()])
        inlines.append(new_line)
      infile.close()

      beacon_lines = []

      # Parse the CSV data; Strip extra spaces from the data.
      beacon_reader = csv.reader(inlines)
      for row in beacon_reader:
        # Strip extra spaces from each field and append to beacon_lines
        beacon_lines.append([field.strip() for field in row])
    elif filename.endswith('.csv'):
      # Read the file into a list first so that invalid SSIDs can be handled
      infile = open(os.path.join(path, filename), 'r')
      inlines = []

      for i,line in enumerate(infile):
        if len(line.strip()) == 0:
          if i == 0:
            # Blank at the start of the file which is normal
            continue
          if i > 2:
            # Second blank line happens after the initial summary block.
            # The rest of the file is of no concern.
            break
        new_line = ''.join([ch for ch in line if ch.isprintable()])
        inlines.append(new_line)
      infile.close()
      
      bssid_to_essid = dict()

      # Parse the file using the CSV reader to create a lookup table for the
      # ESSID
      summary_reader = csv.reader(inlines)
      summary_reader.__next__()   # Dump the first row of the file (the header)
      for row in summary_reader:
        bssid_to_essid[row[SUMMARY_BSSID].strip()] = row[SUMMARY_ESSID].strip()

  # Replace the SSID in each beacon_lines list with the summary's version of
  # the SSID.
  for i,beacon_line in enumerate(beacon_lines):
    if i == 0:
      # Skip the field header row
      continue
    # If line has too few fields, garbage was saved to the ESSID field
    if len(beacon_line) < 7:
      continue
    beacon_essid_length = int(beacon_line[BEACONS_ESSID_LENGTH])
    beacon_essid = beacon_line[BEACONS_ESSID][:beacon_essid_length]
    beacon_bssid = beacon_line[BEACONS_BSSID]
    if beacon_bssid not in bssid_to_essid:
      # If we can't find the ESSID in the summary data, truncate what we do
      # have to the proper length.
      beacon_lines[i][BEACONS_ESSID] = beacon_essid
    else:
      summary_essid = bssid_to_essid[beacon_bssid]
      beacon_lines[i][BEACONS_ESSID] = summary_essid[:beacon_essid_length]

  # Dump the in-memory "file" to the console"
  #prepended_fields = ['capture_user', 'capture_location']
  #print(','.join(prepended_fields + beacon_lines[0]))

  # Create an in-memory "file"
  outfile = io.StringIO()
  outwriter = csv.writer(outfile)

  # Dump field list
  prepended_data = [capture_user, capture_location]
  outwriter.writerows([prepended_data+fields for fields in beacon_lines[1:] if len(fields) > 0])

  outfile.seek(0)
  print('\n'.join([line.strip() for line in outfile.readlines()]))


def main(argv):
  args = parse_args(argv)

  if args.new:
    output_headers()
  elif args.path:
    # TODO: check path exists
    # TODO: check files exist
    # TODO: check files are okay
    rewrite_input_files(args.path)

if __name__=='__main__':
  main(sys.argv)
