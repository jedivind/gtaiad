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

const double ratio_epsilon = 0.000001;
const double dis_epsilon = 1;

int keyCompare(const map<string,int> &, const map<string, int> &, double &, double &);

void keyComparison(map<string, map<string, int> > & fingerprint, map<string, int> & test_finger, set<string> & closestLoc)
{
	map<string, map<string, int> >::const_iterator it;
	double maxRatio = 0, minDis = 9999;
	double curRatio, curDis;
	for(it=fingerprint.begin(); it!=fingerprint.end(); it++){
		if(keyCompare(it->second, test_finger, curRatio, curDis) >= 3){
			if((curRatio > maxRatio + ratio_epsilon) || (abs(curRatio-maxRatio)<ratio_epsilon && curDis<minDis-dis_epsilon)){
				closestLoc.clear();
				maxRatio = curRatio;
				minDis = curDis;
				closestLoc.insert(it->first);
			}
			if(abs(curRatio-maxRatio) < ratio_epsilon && abs(curDis-minDis) < dis_epsilon){
				closestLoc.insert(it->first);
				if(curDis < minDis) minDis = curDis;
			}
				
		}
	}	
}

/*
This function return the number of common mac address. 
Ratio is (A intersect B)/(A union B). And the aveDistance is based on pairwise fingerprint.
*/
int keyCompare(const map<string, int> & ref, const map<string, int> & cur, double &ratio, double &aveDistance) 
{
	int numCommonStr = 0;
	int distance = 0;
	map<string, int>::const_iterator it1, it2, itt1, itt2;
	for(it1=ref.begin(); it1!=ref.end(); it1++){
		if((itt1 = cur.find(it1->first)) != cur.end()){
			numCommonStr ++;
			for(it2=it1, it2++; it2!=ref.end(); it2++){
				if((itt2 = cur.find(it2->first)) != cur.end()){
					int temp=((it1->second - it2->second)-(itt1->second-itt2->second));
					distance += temp*temp;
				}
			}
		}
	}
	ratio = numCommonStr*1.0/(ref.size()+cur.size()-numCommonStr);
	aveDistance = sqrt(distance)/numCommonStr;
	
	return numCommonStr;
					
}



//Order by signal strength, descending direction
bool mySigMacCompare(const pair<int, string> & p1, const pair<int, string> & p2){
	return p1.first > p2.first;
}

void PickStrongFingerprint(map<string, vector<pair<int, string> > > & locSigMac, map<string, map<string, int> > & fingerprint,
                             int maxNum, int maxDiff){
	map<string, vector<pair<int, string> > >::iterator it;
	for(it=locSigMac.begin(); it != locSigMac.end(); it++){
		vector<pair<int, string> > & v = it->second;
		sort(v.begin(), v.end(), mySigMacCompare);
		for(int i=0; i<maxNum && i<v.size(); i++)
			if(v.front().first - v[i].first <= maxDiff){
				fingerprint[it->first][v[i].second] = v[i].first;
			}
	}
}	

void PickStrongFingerprint(vector<pair<int, string> >& vecSigMac, map<string, int> & singleFingerprint, int maxNum, int maxDiff){
	sort(vecSigMac.begin(), vecSigMac.end(), mySigMacCompare);
	for(int i=0; i<maxNum && i<vecSigMac.size(); i++)
		if(vecSigMac.front().first - vecSigMac[i].first <= maxDiff){
			singleFingerprint[vecSigMac[i].second] = vecSigMac[i].first;
		}
}

int main(int argc, const char * argv[])
{
	if(argc != 3){
		cout << "Usage: " << argv[0] << "  some_median.csv" << "  test_data_median.csv "<< endl;
		return 0;
	}
	ifstream data(argv[1]);
	ifstream test_data(argv[2]);
	string line;
	map<string, map<string, int> > fingerprint;
	
	//pair<int, int> stores <signal strength, mac address>. This data structure is used for ordering.
	map<string, vector<pair<int, string> > > locSigMac;
	//Load the fingerprint file (**_median.csv), store it in a map<string, vector<pair<int, string> > > structure
	while(getline(data, line)){
		stringstream lineStream(line);
		string location;
		string macAdd;
		string sigStrMed;
		getline(lineStream, location, ',');
		getline(lineStream, macAdd, ',');
		getline(lineStream, sigStrMed);

		pair<int, string> tempPair(atoi(sigStrMed.c_str()), macAdd);
		locSigMac[location].push_back(tempPair);		
	}
	data.close();

	vector<pair<int, string> > test_sigmac;
	map<string, int> test_fingerprint;
	string location;
	while(getline(test_data, line)){
		stringstream lineStream(line);
		string macAdd;
		string sigStrMed;
		getline(lineStream, location, ',');
		getline(lineStream, macAdd, ',');
		getline(lineStream, sigStrMed);
		pair<int, string> tempPair(atoi(sigStrMed.c_str()), macAdd);
		test_sigmac.push_back(tempPair);
	}
	cout << location << endl;
	PickStrongFingerprint(locSigMac, fingerprint, 10, 5);
	PickStrongFingerprint(test_sigmac, test_fingerprint, 10, 5);
	set<string> closestLoc;
	keyComparison(fingerprint, test_fingerprint, closestLoc);
	set<string>::iterator setit;
	for(setit=closestLoc.begin(); setit!=closestLoc.end(); setit++)
		cout << *setit << '\t';
	cout << endl;
	cout << endl;
	return 0;
}	

