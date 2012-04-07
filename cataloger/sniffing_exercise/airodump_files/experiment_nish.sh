#! /bin/bash 

echo -n 'name_dir:'
read NAME

for i in {1..9}
do 
	./test_addr lh_all_data_median.csv ${NAME}/2K00${i}.csv locations.csv
done

for i in 15 17 18 19 22 24 26 27 28 31
do 
	./test_addr lh_all_data_median.csv ${NAME}/2K0${i}.csv locations.csv
done



for i in {1..7}
do 
	./test_addr lh_all_data_median.csv ${NAME}/3K00${i}.csv locations.csv
done 

./test_addr lh_all_data_median.csv ${NAME}/3K009.csv locations.csv

for i in 14 15 17 18 20 21 23 25 27 31
do 
	./test_addr lh_all_data_median.csv ${NAME}/3K0${i}.csv locations.csv
done 


