#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <utility>
#include <vector>
#include <map>
#include <algorithm>
#include <QPointf>

using namespace std;

const int MedianInterval = 20;
const int FrequencyRatioCutOff = 5;

/*
 * This function is intended to take a raw huge file, consolidate rows with same 
 * <location, Mac Address> field and compute the median. Save the output file as 
 * the all fingerprint which can be loaded in the future. 
*/
int calculate_positions_from_data(QList< QPair<QString, QPointF> > , QList present_capture)
{
	string outfile = argv[1];
	outfile.insert(outfile.size()-4, "_median"); //before ".csv" insert "_median" as part of file name
	ifstream ifm(QList);
	ofstream ofm(outfile.c_str());
	
	string line;
	//for each <location, MacAdd>, store all the signal strength in a vector
	map<string, map<string, vector<int> > > locMacSig; 
	map<string, map<string, vector<int> > >::iterator locMacSig_It;
	
	//store all the rows in ifm in a LocMacSig map structure
	while(getline(ifm, line)){
		stringstream lineStream(line);
		string location;
		string macAdd;
		int sigStr;
		string temp;
		getline(lineStream, location, ',');
		getline(lineStream, macAdd, ',');
		getline(lineStream, temp);
		sigStr = atoi(temp.c_str());
		locMacSig[location][macAdd].push_back(sigStr);
	}

	vector<int> medianVec, frequencyVec;

	locMacSig_It = locMacSig.begin();
	for(; locMacSig_It != locMacSig.end(); locMacSig_It++){
		//Now inside the loop, it's for a single location
		map<string, vector<int> > & macSig = locMacSig_It -> second;
		map<string, vector<int> >::iterator macSig_It;
		medianVec.clear();
		frequencyVec.clear();

		for(macSig_It = macSig.begin(); macSig_It != macSig.end(); macSig_It ++){
			vector<int> & sigVec = macSig_It->second;
			sort(sigVec.begin(), sigVec.end()); //sort the vector in order to compute median
			int median = *(sigVec.begin() + sigVec.size()/2);
			medianVec.push_back(median);
			frequencyVec.push_back(sigVec.size());
		}
		sort(medianVec.begin(), medianVec.end());
		sort(frequencyVec.begin(), frequencyVec.end());
 		int medianCutOff = medianVec.back()-MedianInterval;
		int frequencyCutOff = frequencyVec.back()/FrequencyRatioCutOff;

		for(macSig_It = macSig.begin(); macSig_It != macSig.end(); macSig_It ++){
			vector<int> & sigVec = macSig_It->second;
			int median = *(sigVec.begin() + sigVec.size()/2);
			if(median >= medianCutOff && sigVec.size() >= frequencyCutOff){
				ofm << locMacSig_It->first << ','
			    	<< macSig_It->first << ','
			    	<< median << endl;
			}
		}	
	
	}
	

	ifm.close();
	ofm.close();
	return 0;
}	
