file_date=$(date +%m%d%H%M)
mkdir advanced_internet_files
mkdir advanced_internet_files/$file_date'nishith'
#printf '%s' $file_date
./src/airodump-ng -f 1000 -w advanced_internet_files/$file_date'nishith'/$file_date --channel 1,6,11 wlan0
