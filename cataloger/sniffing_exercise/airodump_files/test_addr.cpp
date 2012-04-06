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
const int doorDisSquare = (1167-1135)*(1167-1135)+(2180-2081)*(2180-2081); //between 2K008 and 2K009

//interval for pixels when projecting the point onto the corridor
const int Interval = 10; 

const int l1start=686;
const int l1end=1226;
const int l2start=1033;
const int l2end=1437;
const int l3start=1437;
const int l3end=2228;
const int l4start=1287;
const int l4end=2103;
const int l5start=1679;
const int l5end=1801;
const int l6start=2006;
const int l6end=2228;

double l1(double x){
	return -0.0259*x+2356.8;
}
double l2(double x){
	return 0.0056*x*x-16.5938*x+13475;
}

double l3(double x){
	return 0.5837*x+412.4187;
}

double l4(double x){
	return 0.5908*x+680.3284;
}

double l5(double x){
	return -1.6311*x+4404.7;
}

double l6(double x){
	return -1.6802*x+5456.4;
}

double DisSquareTwoPts(double x1, double y1, double x2, double y2){
	return (x1-x2)*(x1-x2)+(y1-y2)*(y1-y2);
}

//Given any point (cur_x, cur_y), find the cloest point on the corridor and also return the distance 
double FindPointInCorridor(int cur_x, int cur_y, int & x, int & y){
	double minDisSquare = 999999999;
	double tempx, tempy, tempD;
	for(tempx=l1start; tempx <= l1end; tempx += Interval){
		tempy = l1(tempx);
		if((tempD=DisSquareTwoPts(cur_x, cur_y, tempx, tempy)) < minDisSquare){
			minDisSquare = tempD;
			x=tempx;
			y=tempy;
		}
	}
	
	for(tempx=l2start; tempx <= l2end; tempx += Interval){
		tempy = l2(tempx);
		if((tempD=DisSquareTwoPts(cur_x, cur_y, tempx, tempy)) < minDisSquare){
			minDisSquare = tempD;
			x=tempx;
			y=tempy;
		}
	}
	for(tempx=l3start; tempx <= l3end; tempx += Interval){
		tempy = l3(tempx);
		if((tempD=DisSquareTwoPts(cur_x, cur_y, tempx, tempy)) < minDisSquare){
			minDisSquare = tempD;
			x=tempx;
			y=tempy;
		}
	}
	for(tempx=l4start; tempx <= l4end; tempx += Interval){
		tempy = l4(tempx);
		if((tempD=DisSquareTwoPts(cur_x, cur_y, tempx, tempy)) < minDisSquare){
			minDisSquare = tempD;
			x=tempx;
			y=tempy;
		}
	}
	for(tempx=l5start; tempx <= l5end; tempx += Interval){
		tempy = l5(tempx);
		if((tempD=DisSquareTwoPts(cur_x, cur_y, tempx, tempy)) < minDisSquare){
			minDisSquare = tempD;
			x=tempx;
			y=tempy;
		}
	}
	for(tempx=l6start; tempx <= l6end; tempx += Interval){
		tempy = l6(tempx);
		if((tempD=DisSquareTwoPts(cur_x, cur_y, tempx, tempy)) < minDisSquare){
			minDisSquare = tempD;
			x=tempx;
			y=tempy;
		}
	}
	return sqrt(minDisSquare);	
} 

/*
This function return the number of common mac address and also set the value for ratio and aveDistance. 
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

//This function return the aveDistance. 
double EuKeyCompare(const map<string, int> & ref, const map<string, int> & cur) 
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
	return sqrt(distance)/numCommonStr;
}

//Put the closest location in the set<string> 
void EuKeyComparison(map<string, map<string, int> > & fingerprint, map<string, int> & test_finger, set<string> & closestLoc)
{
	map<string, map<string, int> >::const_iterator it;
	double  minDis = 999999;
	double curDis;
	string loc;
	for(it=fingerprint.begin(); it!=fingerprint.end(); it++){
		if((curDis = EuKeyCompare(it->second, test_finger)) < minDis){
			minDis = curDis;
			loc = it->first;
		}
	}
	closestLoc.insert(loc);	
}
//This function return the cosine similarity. 
double SimKeyCompare(const map<string, int> & ref, const map<string, int> & cur) 
{
	int numCommonStr = 0;
	map<string, int>::const_iterator it1, it2, itt1, itt2;
	for(it1=ref.begin(); it1!=ref.end(); it1++){
		if((itt1 = cur.find(it1->first)) != cur.end()){
			numCommonStr ++;
		}
	}
	return numCommonStr/(sqrt(ref.size())*sqrt(cur.size()));
}

//Put the closest location in the set<string> 
void SimKeyComparison(map<string, map<string, int> > & fingerprint, map<string, int> & test_finger, set<string> & closestLoc)
{
	map<string, map<string, int> >::const_iterator it;
	double  maxSim = -999;
	double curSim;
	string loc;
	for(it=fingerprint.begin(); it!=fingerprint.end(); it++){
		if((curSim = SimKeyCompare(it->second, test_finger)) > maxSim){
			maxSim = curSim;
			loc = it->first;
		}
	}
	closestLoc.insert(loc);	
}


double BestPt(map<string, pair<int, int> > & locMap, set<string> & closestLoc, int & x, int & y, int& floor){
	int numFloorTwo=0, numFloorThree=0;
	set<string>::iterator set_it;
	for(set_it = closestLoc.begin(); set_it != closestLoc.end(); ++set_it){
		if((*set_it).find("2K") == string::npos) numFloorThree ++;
		else numFloorTwo ++;
	}
	int cur_x=0, cur_y=0;
	if(numFloorThree > numFloorTwo){
		floor = 3;
		for(set_it=closestLoc.begin(); set_it!=closestLoc.end(); ++set_it){
			if((*set_it).find("3K") != string::npos){
				pair<int, int> temp = locMap[*set_it];
				cur_x += temp.first;
				cur_y += temp.second;
			}
		}
		cur_x /= numFloorThree;
		cur_y /= numFloorThree;
		if(numFloorThree <= 3) { x = cur_x; y = cur_y;}
		else return FindPointInCorridor(cur_x, cur_y, x, y);
	}	
	if(numFloorThree < numFloorTwo){
		floor = 2;
		for(set_it=closestLoc.begin(); set_it!=closestLoc.end(); ++set_it){
			if((*set_it).find("2K") != string::npos){
				pair<int, int> temp = locMap[*set_it];
				cur_x += temp.first;
				cur_y += temp.second;
			}
		}
		cur_x /= numFloorTwo;
		cur_y /= numFloorTwo;
		if(numFloorTwo <= 3) { x = cur_x; y = cur_y;}
		else return FindPointInCorridor(cur_x, cur_y, x, y);
	}	
	if(numFloorThree == numFloorTwo) {floor = 0;  return 99999;}
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
	if(argc != 4){
    /* some_median.csv - building fingerprints file
     * test_data_median.csv - location fingerprint
     * locations.csv  - the file which stores all the coordinates for each location
     */
		cout << "Usage: " << argv[0] << "  some_median.csv" << "  test_data_median.csv "<< " locations.csv" << endl;
		return 0;
	}
	ifstream data(argv[1]);
	ifstream test_data(argv[2]);
	ifstream loc_data(argv[3]);
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
		locSigMac[location].push_back(pair<int, string> (atoi(sigStrMed.c_str()), macAdd) );		
	}

	//Here is to read the fingerprint for test location
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
		test_sigmac.push_back(pair<int,string> (atoi(sigStrMed.c_str()), macAdd));
	}
	cout << location << endl;

        //Read all the <location, <x,y> > in a map data structure
	map<string, pair<int, int> > locMap;
	getline(loc_data, line); //ignore the header
	while(getline(loc_data, line)){
		stringstream lineStream(line);
		string loc, numFloor, numX, numY;
		getline(lineStream, loc, ';');
		getline(lineStream, numFloor, ';');
		getline(lineStream, numX, ';');
		getline(lineStream, numY);
		locMap[loc] = pair<int, int>(atoi(numX.c_str()), atoi(numY.c_str()));
	}
		
	PickStrongFingerprint(locSigMac, fingerprint, 10, 5);
	PickStrongFingerprint(test_sigmac, test_fingerprint, 10, 5);
	set<string> closestLoc;
	SimKeyComparison(fingerprint, test_fingerprint, closestLoc);

	int x, y, floor;
	double dis = BestPt(locMap, closestLoc, x, y, floor);
	if(floor != 0){
		double error = sqrt((locMap[location].first-x)*(locMap[location].first-x)+(locMap[location].second-y)*(locMap[location].second-y));
		cout << floor << '\t' << error/sqrt(doorDisSquare) << '\t';
		set<string>::iterator set_it;
		for(set_it=closestLoc.begin(); set_it!=closestLoc.end(); set_it++)
			cout << *set_it << "  ";
		cout << endl;
	}else{
		cout << "Cannot decide floor number" << endl;
	}
	return 0;
}	

