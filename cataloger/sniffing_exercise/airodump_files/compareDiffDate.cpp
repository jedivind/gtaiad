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

const int WEAKEST = -90;

//print out the fingerprint for one location
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
double distanceFing(const map<string, int> & fing1, const map<string, int> & fing2)
{
	int res = 0;
	map<string, int>::const_iterator it1 = fing1.begin(), it2;
	for(; it1 != fing1.end(); it1 ++){
		it2 = fing2.find(it1->first);
		if(it2 != fing2.end())//common mac address
			res += (it1->second-it2->second)*(it1->second-it2->second);
		else
			res += (it1->second-WEAKEST)*(it1->second-WEAKEST);
	}
	it2 = fing2.begin();
	for(; it2 != fing2.end(); it2++){
		if(fing1.find(it2->first) == fing1.end())
			res += (it2->second-WEAKEST)*(it2->second-WEAKEST);
	}
	return sqrt(res*1.0);	
}

void compareFingerprint(map<string, map<string, int> > & fingerprint1, map<string, map<string, int> > & fingerprint2){
	ofstream os("temp2.csv");
	map<string, map<string, int> >::const_iterator finger_It1, finger_It2;	
	
	//print first row, the labels
	os << ',';
	for(finger_It2 = fingerprint2.begin(); finger_It2 != fingerprint2.end(); finger_It2 ++)
		os << finger_It2->first << ',';
	os << endl;

	for(finger_It1 = fingerprint1.begin(); finger_It1 != fingerprint1.end(); finger_It1 ++){
		os << finger_It1->first << ',';
		for(finger_It2 = fingerprint2.begin(); finger_It2 != fingerprint2.end(); finger_It2 ++)
			os<< distanceFing(finger_It1->second, finger_It2->second) << ',';
		os << endl;
	}
}

int main(int argc, const char * argv[])
{
	if(argc != 3){
		cout << "Usage: " << argv[0] << "some_median1.csv some_median2.csv" << endl;
		return 0;
	}
	ifstream data1(argv[1]), data2(argv[2]);
	
	string line;
	map<string, map<string, int> > fingerprint1, fingerprint2;

	//Load the fingerprint file (**_median.csv), store it in a map<string, map<string, int> > structure
	while(getline(data1, line)){
		stringstream lineStream(line);
		string location;
		string macAdd;
		string sigStrMed;
		string temp;
		getline(lineStream, location, ',');
		getline(lineStream, macAdd, ',');
		getline(lineStream, sigStrMed);
		fingerprint1[location][macAdd] = atoi(sigStrMed.c_str());
	}
	data1.close();

	while(getline(data2, line)){
		stringstream lineStream(line);
		string location;
		string macAdd;
		string sigStrMed;
		string temp;
		getline(lineStream, location, ',');
		getline(lineStream, macAdd, ',');
		getline(lineStream, sigStrMed);
		fingerprint2[location][macAdd] = atoi(sigStrMed.c_str());
	}
	data2.close();

	compareFingerprint(fingerprint1, fingerprint2);
	return 0;
}	
