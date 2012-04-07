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

for i in 15 17 18 19 22 24 26 27 28 31  
do
	grep "2K0${i}," $DATAFILE > ${NAME}/2K0${i}.csv
done

for i in 14 15 17 18 20 21 23 25 27 31
do
	grep "3K0${i}," $DATAFILE > ${NAME}/3K0${i}.csv
done

 

