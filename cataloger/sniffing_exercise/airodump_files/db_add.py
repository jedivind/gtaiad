"Add the specified ssids from a consolidated scan .csv median-power-filtered file to the database."

try:
  import argparse
except (ImportError):
  import argparse32 as argparse

import csv
import sqlite3
import sys
import os
import pdb

USAGE_EPILOG = """\
  Example: python calc_medians.py < pk_0226_data.csv | python db_add.py --ssid GTwpa,GTwireless,GTvisitor --debug klaus.db
"""

def parse_args(argv):
  "Parse arguments on the command line"

  parser = argparse.ArgumentParser(
      description = 'Add fingerprint data to database\n\n' + USAGE_EPILOG,
      formatter_class=argparse.RawDescriptionHelpFormatter)

  parser.add_argument('db',
      help='sqlite3 database file')

  parser.add_argument('--ssid', required=True,
      help='add specified ssids to the database')

  parser.add_argument('--debug', action='store_true',
      help='do not change the database')

  return parser.parse_args()

def known_locations(db):
  c = db.cursor()

  c.execute('select capture_location_name from capture_locations order by capture_location_name')

  capture_location_names = [str(fields[0]) for fields in c]
  return capture_location_names

def read_fingerprints():
  reader = csv.reader(sys.stdin)

  header = reader.next()

  rows = [row for row in reader]

  return header,rows

def main(argv):
  args = parse_args(argv)


  ESSIDS_TO_PASS = args.ssid.split(',')

  if not os.path.exists(args.db):
    print 'Database %s does not exist' % args.db
    sys.exit(1)

  db = sqlite3.connect(args.db)

  available_loc_ids = known_locations(db)

  header,rows = read_fingerprints()

  LOC_ID = header.index('capture_location')
  CHANNEL = header.index('channel')
  ESSID = header.index('ESSID')
  BSSID = header.index('BSSID')
  MED_POWER = header.index('median_power')

  debug_missing_locs = []

  for row in rows:
    if row[ESSID] in ESSIDS_TO_PASS:
      if args.debug:
        if row[LOC_ID] not in available_loc_ids:
          if row[LOC_ID] in debug_missing_locs:
            continue
          else:
            print 'Location %s not in the database' % row[LOC_ID]
            debug_missing_locs.append(row[LOC_ID])
            continue
      else:
        if row[LOC_ID] not in available_loc_ids:
          print 'Location %s not in the database' % row[LOC_ID]
          db.rollback()
          sys.exit(1)

        cur = db.cursor()
        res = cur.execute('''insert into capture_fingerprints (
            capture_location_name, capture_fingerprint_bssid,
            capture_fingerprint_channel, capture_fingerprint_median_power,
            capture_fingerprint_ssid) values (
            ?, ?, ?, ?, ?)''',
            (row[LOC_ID], row[BSSID],
            row[CHANNEL], row[MED_POWER],
            row[ESSID]))
  db.commit()

if __name__=='__main__':
  main(sys.argv)
