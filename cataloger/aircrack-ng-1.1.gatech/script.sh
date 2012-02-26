file_date=$(date +%m%d%H%M)
echo -n 'User: '
read UNAME
echo -n 'Location tag: '
read LOCTAG
DPATH=advanced_internet_files/$(date +%m%d)-$UNAME/$file_date-$UNAME-$LOCTAG
mkdir -p $DPATH
#printf '%s' $file_date
echo ./src/airodump-ng -f 20000 -w $DPATH/$LOCTAG --channel 1,6,11 --beacons wlan0
