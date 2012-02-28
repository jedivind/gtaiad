import csv
import io
import sys

# Requires Python 2.7 or Python 3
#
# Example:
#   python field_rm.py 1 4 6 8 < pk_data.csv

def main(argv):
  if len(argv) < 2:
    print('Usage: python field_rm.py COL [COL...]')

  columns_to_remove = [int(arg) for arg in argv[1:]]

  reader = csv.reader(sys.stdin)
  writer = csv.writer(sys.stdout)

  # Remove duplicates and reverse sort
  columns_to_remove = sorted(list(set(columns_to_remove)), reverse=True)

  for row in reader:
    if not row:
      continue
    [row.pop(col - 1) for col in columns_to_remove]
    writer.writerow(row)

if __name__=='__main__':
  try:
    main(sys.argv)
  except(IOError, KeyboardInterrupt):
    pass
