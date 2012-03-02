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

const int WEAKEST = -85;

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

int main(int argc, const char * argv[])
{
	if(argc != 2){
		cout << "Usage: " << argv[0] << "some_median.csv" << endl;
		return 0;
	}
	ifstream data(argv[1]);
	string line;
	map<string, map<string, int> > fingerprint;
	map<string, map<string, int> >::const_iterator finger_It, finger_Itt;

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
	}

	//printLoc(fingerprint, "2K001");
	finger_It = fingerprint.begin();
	finger_Itt = finger_It;
	for(finger_It = fingerprint.begin(); finger_It != fingerprint.end(); finger_It ++)
		for(finger_Itt = fingerprint.begin(); finger_Itt != fingerprint.end(); finger_Itt ++){
			cout << finger_It->first << '\t' << finger_Itt->first << '\t' 
				<< distanceFing(finger_It->second, finger_Itt->second) << endl;
	}

	data.close();
	return 0;
}	
