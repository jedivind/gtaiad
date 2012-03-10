#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <utility>
#include <vector>
#include <map>
#include <algorithm>
#include <cmath>

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

//Given the fingerprints for two location, compute the Euclidean distance
//  location_fingerprint = map< MAC_ADDRESS, MEDIAN_SIGNAL_STRENGTH >
double distanceFing_No_Diff(const map<string, int> & fing1, const map<string, int> & fing2)
{
	int res = 0;
	map<string, int>::const_iterator it1, it2;
	int weakestFing1 = 0, weakestFing2 = 0;
	int num=0;
	for(it1 = fing1.begin(); it1 != fing1.end(); it1 ++){
		if(weakestFing1 > it1->second)
			weakestFing1 = it1->second;
	}
	
	for(it2 = fing2.begin(); it2 != fing2.end(); it2 ++){
		if(weakestFing2 > it2->second)
			weakestFing2 = it2->second;
	}
	
	for(it1 = fing1.begin(); it1 != fing1.end(); it1 ++){
		it2 = fing2.find(it1->first);
		if(it2 != fing2.end())//common mac address
			res += (it1->second-it2->second)*(it1->second-it2->second);
		else
			res += (it1->second-weakestFing1)*(it1->second-weakestFing1);
		num ++;
	}
	for(it2 = fing2.begin(); it2 != fing2.end(); it2++){
		if(fing1.find(it2->first) == fing1.end()){
			res += (it2->second-weakestFing2)*(it2->second-weakestFing2);
			num ++;
		}
	}
	return sqrt(res*1.0/num);	
}

double pairwiseDis(map<string, pair<int, int> > & fingpair)
{
	map<string, pair<int, int> >::const_iterator it1, it2;
	int res = 0;
	int num = 0;
	for(it1 = fingpair.begin(); it1 != fingpair.end(); it1 ++)
		for(it2=it1, it2++; it2 != fingpair.end(); it2 ++){
			int temp = (it1->second.first-it2->second.first)-(it1->second.second-it2->second.second);
			res += temp*temp;
			num ++;
		}
	return sqrt(res * 1.0/num);

} 
//Using difference between a pair of AP, instead of absolute values
double distanceFing_Diff(const map<string, int> & fing1, const map<string, int> & fing2)
{
	map<string, int>::const_iterator it1, it2;
	int weakestFing1 = 0, weakestFing2 = 0;
	for(it1 = fing1.begin(); it1 != fing1.end(); it1 ++){
		if(weakestFing1 > it1->second)
			weakestFing1 = it1->second;
	}
	
	for(it2 = fing2.begin(); it2 != fing2.end(); it2 ++){
		if(weakestFing2 > it2->second)
			weakestFing2 = it2->second;
	}
	
	map<string, pair<int, int> > fingpair;
	
	for(it1 = fing1.begin(); it1 != fing1.end(); it1 ++){
		fingpair[it1->first].first = it1->second;
		fingpair[it1->first].second = weakestFing2;
	}
	
	for(it2 = fing2.begin(); it2 != fing2.end(); it2++){
		if(fingpair.find(it2->first) == fingpair.end()){
			fingpair[it2->first].first = weakestFing1;
			fingpair[it2->first].second = it2->second;
		}else{
			fingpair[it2->first].second = it2->second;
		}
	}
	return pairwiseDis(fingpair);	
}


void printFingerprint(map<string, map<string, int> > & fingerprint){
	//ofstream os("temp_03_09_2.csv");
	map<string, map<string, int> >::const_iterator finger_It1, finger_It2;	
	
	//print first row, the labels
	cout << ',';
	for(finger_It1 = fingerprint.begin(); finger_It1 != fingerprint.end(); finger_It1 ++)
		cout << finger_It1->first << ',';
	cout << endl;

	for(finger_It1 = fingerprint.begin(); finger_It1 != fingerprint.end(); finger_It1 ++){
		cout << finger_It1->first << ',';
		for(finger_It2 = fingerprint.begin(); finger_It2 != fingerprint.end(); finger_It2 ++)
			cout<< distanceFing_No_Diff(finger_It1->second, finger_It2->second) << ',';
		cout << endl;
	}
}

//output all the pairs <locationID, #(access points captured) 
void printLocNumAP(map<string, map<string, int> > & fingerprint){
	map<string, map<string, int> >::const_iterator finger_It;	
	
	for(finger_It = fingerprint.begin(); finger_It != fingerprint.end(); finger_It ++)
		cout << finger_It->first << ',' << finger_It->second.size() << endl;
}

bool mySigMacidCompare(const pair<int, int> & p1, const pair<int, int> & p2){
	return p1.first > p2.first;
}

void clusterFingerprint(map<string, vector<pair<int, int> > > & locSigMacid, int maxNum){
	map<string, vector<pair<int, int> > >::iterator it;
	for(it=locSigMacid.begin(); it != locSigMacid.end(); it++){
		vector<pair<int, int> > & v = it->second;
		sort(v.begin(), v.end(), mySigMacidCompare);
		cout << it->first << ",";
		for(int i=0; i<maxNum && i<v.size(); i++)
			cout << v[i].second << ",";
		cout << endl;
	}
	
}

int main(int argc, const char * argv[])
{
	if(argc != 2){
		cout << "Usage: " << argv[0] << "some_median.csv" << endl;
		return 0;
	}
	ifstream data(argv[1]);
	string line;
	map<string, map<string, int> > fingerprint;
	map<string, int> allMacAddr; //Storing the macAdd and their index
	map<string, vector<pair<int, int> > > locSigMacid; 
	int index = 0;
	//Load the fingerprint file (**_median.csv), store it in a map<string, map<string, int> > structure
	while(getline(data, line)){
		stringstream lineStream(line);
		string location;
		string macAdd;
		string sigStrMed;
		string temp;
		getline(lineStream, location, ',');
		getline(lineStream, macAdd, ',');
		getline(lineStream, sigStrMed);
		fingerprint[location][macAdd] = atoi(sigStrMed.c_str());

		if(allMacAddr.find(macAdd) == allMacAddr.end()) allMacAddr[macAdd] = index ++;
		pair<int, int> tempPair(atoi(sigStrMed.c_str()), allMacAddr[macAdd]);
		locSigMacid[location].push_back(tempPair);		
	}
	data.close();
	clusterFingerprint(locSigMacid, 10);
	//printFingerprint(fingerprint);
	return 0;
}	

