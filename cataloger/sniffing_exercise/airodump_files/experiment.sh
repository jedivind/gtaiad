#! /bin/bash 

echo -n 'name_dir:'
read NAME

for i in {1..9}
do 
	./test_addr lh_all_data_median.csv ${NAME}/2K00${i}.csv
done

for i in {1..7}
do 
	./test_addr lh_all_data_median.csv ${NAME}/3K00${i}.csv
done 

./test_addr lh_all_data_median.csv ${NAME}/3K009.csv

