#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <utility>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;


const int CutOff = 3; //Cut off the lower 1/3

/*
 * This function is intended to take a raw huge file, consolidate rows with same 
 * <location, Mac Address> field and compute the median. Save the output file as 
 * the all fingerprint which can be loaded in the future. 
 * **********************************************************
 * For one location, <Mac Address, median, frequency>, filter out low median and low frequency ones. 
*/
int main(int argc, const char * argv[])
{
	if(argc != 2){
		cout << "Usage: " << argv[0] << "somerawfile.csv" << endl;
		return 0;
	}
	string outfile = argv[1];
	outfile.insert(outfile.size()-4, "_median"); //before ".csv" insert "_median" as part of file name
	ifstream ifm(argv[1]);
	ofstream ofm(outfile.c_str());
	
	string line;
	//for each <location, MacAdd>, store all the signal strength in a vector
	map<string, map<string, vector<int> > > locMacSig; 
	map<string, map<string, vector<int> > >::iterator locMacSig_It;
	
	//Each line contains location_ID, mac_address, signal_strength
	while(getline(ifm, line)){
		stringstream lineStream(line);
		string location;
		string macAdd;
		string sigStr;
		getline(lineStream, location, ',');
		getline(lineStream, macAdd, ',');
		getline(lineStream, sigStr);
		locMacSig[location][macAdd].push_back(atoi(sigStr.c_str()));
	}

	vector<int> medianVec, frequencyVec;

	locMacSig_It = locMacSig.begin();
	for(; locMacSig_It != locMacSig.end(); locMacSig_It++){
		//Now inside the loop, it's for a single location
		map<string, vector<int> > & macSig = locMacSig_It -> second;
		map<string, vector<int> >::iterator macSig_It;
		medianVec.clear(); //stores all the medians for different mac addresses for a single location
		frequencyVec.clear(); //stores the number of occurrences for different mac addresses for a single location

		for(macSig_It = macSig.begin(); macSig_It != macSig.end(); macSig_It ++){
			vector<int> & sigVec = macSig_It->second;
			sort(sigVec.begin(), sigVec.end()); //sort the vector in order to compute median
			int median = *(sigVec.begin() + sigVec.size()/2);
			medianVec.push_back(median);
			frequencyVec.push_back(sigVec.size());
		}
		//Sort them in ascending order
		sort(medianVec.begin(), medianVec.end());
		sort(frequencyVec.begin(), frequencyVec.end());
	
 		int medianCutOff = ((*(medianVec.begin() + medianVec.size()/2))*2+medianVec.front())/3;
		int frequencyCutOff = ((*(frequencyVec.begin() + frequencyVec.size()/2))*2+frequencyVec.front())/3;

		//Only output the <mac address, median> when they exceed ***cutoff
		for(macSig_It = macSig.begin(); macSig_It != macSig.end(); macSig_It ++){
			vector<int> & sigVec = macSig_It->second;
			int median = *(sigVec.begin() + sigVec.size()/2);
			
			//if(median >= medianCutOff && sigVec.size() >= frequencyCutOff){
				ofm << locMacSig_It->first << ','
			    	<< macSig_It->first << ','
			    	<< median << endl;
			//}
 			/*
			ofm << locMacSig_It->first << ','
			   <<macSig_It->first << ','
			   << median << ','
			  << sigVec.size() << endl;
			*/
		}	
	
	}
	
	ifm.close();
	ofm.close();
	return 0;
}	
