#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <utility>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;

/*
 * This function is intended to take a raw huge file, consolidate rows with same 
 * <location, Mac Address> field and compute the median. Save the output file as 
 * the all fingerprint which can be loaded in the future. 
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
	map<pair<string, string>, vector<int> > locMacSig; 
	map<pair<string, string>, vector<int> >::iterator locMacSig_It;
	
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
		pair<string, string> cur(location, macAdd);
		locMacSig[cur].push_back(sigStr);
	}

	locMacSig_It = locMacSig.begin();
	for(; locMacSig_It != locMacSig.end(); locMacSig_It++){
		vector<int> & sigVec = locMacSig_It->second;
		sort(sigVec.begin(), sigVec.end()); //sort the vector in order to compute median
		const pair<string, string> & locmac = locMacSig_It->first;
		int median = *(sigVec.begin() + sigVec.size()/2);
		ofm << locmac.first << ',';
		ofm << locmac.second << ',';
		ofm << median << endl;
		
	}

	ifm.close();
	ofm.close();
	return 0;
}	
