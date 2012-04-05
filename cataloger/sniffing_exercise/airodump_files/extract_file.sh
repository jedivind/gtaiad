#! /bin/bash 

echo -n 'raw file:'
read DATAFILE
echo -n 'name_dir:'
read NAME

mkdir $NAME

for i in {1..7}
do     
	grep "2K00${i}," $DATAFILE > ${NAME}/2K00${i}.csv
	grep "3K00${i}," $DATAFILE > ${NAME}/3K00${i}.csv
done

grep "2K008," $DATAFILE > ${NAME}/2K008.csv
grep "2K009," $DATAFILE > ${NAME}/2K009.csv
grep "3K009," $DATAFILE > ${NAME}/3K009.csv



