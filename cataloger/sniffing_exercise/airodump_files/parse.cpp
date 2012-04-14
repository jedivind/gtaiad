#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <utility>
#include <vector>
#include <map>
#include <algorithm>
#include <cmath>
#include <set>

using namespace std;

//print out all <Mac Addr, signal strength> paris for one location
void printLoc(const map<string, map<string, int> > & fingerprint, string location)
{
	map<string, map<string, int> >::const_iterator it = fingerprint.find(location);
	if(it != fingerprint.end()){
		const map<string, int> & finger = it->second;
		map<string, int>::const_iterator itt = finger.begin();
		for(; itt != finger.end(); itt++)
			cout << itt->first << '\t' << itt->second << endl;
	}else{
		cout << "location doesn't exist\n";
	}

}

//output all the pairs <locationID, #(access points captured) 
void printLocNumAP(map<string, map<string, int> > & fingerprint){
	map<string, map<string, int> >::const_iterator finger_It;	
	
	for(finger_It = fingerprint.begin(); finger_It != fingerprint.end(); finger_It ++)
		cout << finger_It->first << ',' << finger_It->second.size() << endl;
}

//Order by signal strength, descending direction
bool mySigMacidCompare(const pair<int, int> & p1, const pair<int, int> & p2){
	return p1.first > p2.first;
}

void clusterFingerprint(map<string, vector<pair<int, int> > > & locSigMacid, int maxNum, int maxDiff){
	map<string, vector<pair<int, int> > >::iterator it;
	for(it=locSigMacid.begin(); it != locSigMacid.end(); it++){
		vector<pair<int, int> > & v = it->second;
		sort(v.begin(), v.end(), mySigMacidCompare);
		cout << it->first << ",";
		for(int i=0; i<maxNum && i<v.size(); i++)
			if(v.front().first - v[i].first <= maxDiff){
				cout << v[i].second << ",";
			}
		cout << endl;
	}
}

//Load the mac_Index.csv file (contains <mac, index>)  into a map structure 
void LoadMacIndex(map<string, int> & macIndexMap, ifstream & in)
{
	string line;
	while(getline(in, line)){
		stringstream lineStream(line);
		string macAdd;
		string index;
		getline(lineStream, macAdd, ',');
		getline(lineStream, index);
		macIndexMap[macAdd] = atoi(index.c_str());
	}
}

int main(int argc, const char * argv[])
{
	if(argc != 3){
		cout << "Usage: " << argv[0] << "  some_median.csv" << "  mac_index.csv "<< endl;
		return 0;
	}
	ifstream data(argv[1]);
	ifstream macIndexIn(argv[2]);

	string line;
	map<string, map<string, int> > fingerprint;
	map<string, int> macIndexMap; //Storing the macAdd and their index
	LoadMacIndex(macIndexMap, macIndexIn);
	
	//pair<int, int> stores <signal strength, mac address id>
	map<string, vector<pair<int, int> > > locSigMacid; 
	int index = 0;
	//Load the fingerprint file (**_median.csv), store it in a map<string, map<string, int> > structure
	while(getline(data, line)){
		stringstream lineStream(line);
		string location;
		string macAdd;
		string sigStrMed;
		getline(lineStream, location, ',');
		getline(lineStream, macAdd, ',');
		getline(lineStream, sigStrMed);
		fingerprint[location][macAdd] = atoi(sigStrMed.c_str());

		if(macIndexMap.find(macAdd) == macIndexMap.end()) { cout << "No such mac address in file\n"; return 0; }
		pair<int, int> tempPair(atoi(sigStrMed.c_str()), macIndexMap[macAdd]);
		locSigMacid[location].push_back(tempPair);		
	}
	//printCommonMac(locSigMacid);
	clusterFingerprint(locSigMacid, 10, 5);
	return 0;
}	

