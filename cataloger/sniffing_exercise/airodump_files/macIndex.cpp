#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <utility>
#include <vector>
#include <map>
#include <stdlib.h>

using namespace std;

/*
 *This function is intended to take a **_median.csv file (contains locationID, 
 *mac address, signal strength median),
 *extract the new physical addresses(undiscovered before) and add them
 *<Mac Address, index> into the file mac_index.csv
 *Index starts from 1
 */ 
int main(int argc, const char * argv[])
{
	if(argc != 2){
		cout << "Usage: " << argv[0] << "      some_median.csv" << endl;
		return 0;
	}
	
	ifstream macIndexIn("mac_index.csv");
	if(!macIndexIn) {
		cout << "Open mac_index.csv error, this file may not exist in which case create it first\n";
		return 0;
	}

	string line;
	map<string, int > macIndexMap;
	int num= 0;

	//Read what we have in the mac_index.csv file into the map structure
	while(getline(macIndexIn, line)){
		stringstream lineStream(line);
		string macAdd;
		string index;
		getline(lineStream, macAdd, ',');
		getline(lineStream, index);
		macIndexMap[macAdd] = atoi(index.c_str());
		num ++;
	}
	
	macIndexIn.close();
	
	ifstream data(argv[1]);
	ofstream macIndexOut("mac_index.csv", ios::app);
	//Add what we don't have yet
	while(getline(data, line)){
		stringstream lineStream(line);
		string str;
		string macAdd;
		getline(lineStream, str, ',');
		getline(lineStream, macAdd, ',');
		getline(lineStream, str);
		if(macIndexMap.find(macAdd) == macIndexMap.end()){
			//This is a new mac address we never encountered before
			num ++;
			macIndexOut << macAdd << "," << num << endl;
			macIndexMap[macAdd] = num;
		}
	}

	macIndexOut.close();
	data.close();
	return 0;
}	

