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

void clusterFingerprint(map<string, vector<pair<int, int> > > & locSigMacid, int maxNum, int maxDiff){
	map<int, set<string> > macIdLocCluster;
	map<string, vector<pair<int, int> > >::iterator it;
	for(it=locSigMacid.begin(); it != locSigMacid.end(); it++){
		vector<pair<int, int> > & v = it->second;
		sort(v.begin(), v.end(), mySigMacidCompare);
		cout << it->first << ",";
		for(int i=0; i<maxNum && i<v.size(); i++)
			if(v.front().first - v[i].first <= maxDiff){
				cout << v[i].second << ",";
				macIdLocCluster[v[i].second].insert(it->first);
			}
		cout << endl;
	}
	
	map<int, set<string> >::iterator it1, it2, it3;
	for(it1 = macIdLocCluster.begin(); it1 != macIdLocCluster.end(); it1 ++){
		for(it2 = it1, it2 ++; it2 != macIdLocCluster.end(); it2 ++){
			for(it3 = it2, it3++; it3 != macIdLocCluster.end(); it3 ++){
				set<string> temp, cluster;
				set_intersection(it1->second.begin(), it1->second.end(), it2->second.begin(),
						it2->second.end(), inserter(temp, temp.begin()));
			 	set_intersection(temp.begin(), temp.end(), it3->second.begin(),
						it3->second.end(), inserter(cluster, cluster.begin()));
				if(cluster.size() > 2){
					cout << it1->first << "," << it2->first << "," << it3->first << ",";
					set<string>::iterator it;
					for(it = cluster.begin(); it != cluster.end(); it ++)
						cout << *it << ",";
					cout << endl;
				}
			}
		}
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
	data.close();
	clusterFingerprint(locSigMacid, 10, 5);
	return 0;
}	

