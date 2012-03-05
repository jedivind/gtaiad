"""\
Example usage:

  python calc_medians.py < vind_0226_data.csv

Groups input data by capture_location, channel, bssid.  Calculates the
median power of each ESSIDs in the group.  Outputs CSV data with new fields:
median_power and beacons_captured.
"""

import csv
import sys
import pdb
from pprint import pprint

# Requires Python 2.7 (or possibly 2.6, 2.5)

output_field_names = ['capture_location', 'channel', 'BSSID', 'median_power', 'ESSID', 'beacons_captured']

# Structure hash as CAPTURE_LOCATION/CHANNEL/BSSID/ESSID_AND_FIELDS

def main():
  #infile = open('pk_0226_data.csv', 'r')
  infile = sys.stdin
  reader = csv.reader(infile)
  writer = csv.writer(sys.stdout)

  # Get ordered field names from the first line
  field_names = reader.next()

  dict_reader = csv.DictReader(infile, field_names)

  all_data = dict()
  line_num = 1

  # Read full file.  Store using ESSIDs as the key to a dict.  Each line of the
  # file is stored as a list of tuples (trunked_SSID, line_list) within the dict.
  for fields in dict_reader:
    line_num = line_num + 1

    # Ignore when power==-1
    if fields['power'] == '-1':
      continue

    bssid = fields['BSSID']
    capture_location = fields['capture_location']
    channel = int(fields['channel'])
    essid = fields['ESSID'][:int(fields['essid_length'])]

    if capture_location not in all_data.keys():
      all_data[capture_location] = dict()
    if channel not in all_data[capture_location].keys():
      all_data[capture_location][channel] = dict()

    if bssid not in all_data[capture_location][channel].keys():
      all_data[capture_location][channel][bssid] = [essid, [fields]]
    else:
      # Warn if ESSID has changed
      if essid != all_data[capture_location][channel][bssid][0]:
        print >>sys.stderr, 'ESSID changed line %i from %s to %s' % (
            line_num, all_data[capture_location][channel][bssid][0], essid)
      all_data[capture_location][channel][bssid][1].append(fields)

  writer.writerow(output_field_names)

  for location in sorted(all_data.keys()):
    for channel in sorted(all_data[location].keys()):
      output_fields = []
      for bssid in sorted(all_data[location][channel]):
        essid = all_data[location][channel][bssid][0]
        beacons = all_data[location][channel][bssid][1]
        all_powers = sorted([int(fields['power']) for fields in beacons])
        median_power = all_powers[len(all_powers)/2]
        output_fields.append([location, channel, bssid, median_power, essid, len(all_powers)])
      writer.writerows(sorted(output_fields, key=lambda x: x[3], reverse=True))

if __name__=='__main__':
  try:
    main()
  except(IOError, KeyboardInterrupt):
    pass
