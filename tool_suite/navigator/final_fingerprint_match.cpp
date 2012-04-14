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
#include<string.h>

using namespace std;

const int doorDisSquare = (1167-1135)*(1167-1135)+(2180-2081)*(2180-2081); //between 2K008 and 2K009
const int numDoors = 5;

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


//This function return the aveDistance. Euclidean pairwise distance over all the common pairs
double EuKeyCompare(const map<string, int> & ref, const map<string, int> & cur) 
{
	int numCommonStr = 0;
	int distance = 0;
	int numPairs = 0;
	map<string, int>::const_iterator it1, it2, itt1, itt2;
	for(it1=ref.begin(); it1!=ref.end(); it1++){
		if((itt1 = cur.find(it1->first)) != cur.end()){
			numCommonStr ++;
			for(it2=it1, it2++; it2!=ref.end(); it2++){
				if((itt2 = cur.find(it2->first)) != cur.end()){
					int temp=((it1->second - it2->second)-(itt1->second-itt2->second));
					distance += temp*temp;
					numPairs ++;
				}
			}
		}
	}
	if(numPairs > 0) return sqrt(distance/numPairs);
	else return 9999999;
}

//Put the closest three locations in the vector<string> with respect wo EuKeyCompare
void EuKeyComparison(map<string, map<string, int> > & fingerprint, map<string, int> & test_finger, vector<string> & closestLoc)
{
	map<string, map<string, int> >::const_iterator it;
	double  minDis = 999999;
	double curDis;
	string loc1, loc2, loc3;
	string loc;
	for(it=fingerprint.begin(); it!=fingerprint.end(); it++){
		if((curDis = EuKeyCompare(it->second, test_finger)) < minDis){
			minDis = curDis;
			loc = it->first;
		}
	}
	loc1 = loc;
	closestLoc.push_back(loc);
	minDis = 999999;
	for(it=fingerprint.begin(); it!=fingerprint.end(); it++){
		if((curDis = EuKeyCompare(it->second, test_finger)) < minDis){
			if(it->first != loc1){ 
				minDis = curDis;
				loc = it->first;
			}
		}
	}
	closestLoc.push_back(loc);
	loc2 = loc;
	minDis = 999999;
	for(it=fingerprint.begin(); it!=fingerprint.end(); it++){
		if((curDis = EuKeyCompare(it->second, test_finger)) < minDis){
			if(it->first != loc1 && it->first != loc2){
				minDis = curDis;
				loc = it->first;
			}
		}
	}
	closestLoc.push_back(loc);
	loc3 = loc;	
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
//Put the closest three locations in the vector<string> with respect to SimKeyCompare() 
void SimKeyComparison(map<string, map<string, int> > & fingerprint, map<string, int> & test_finger, vector<string> & closestLoc)
{
        map<string, map<string, int> >::const_iterator it;
        double  maxSim = -999;
        double curSim;
        double epsilon = 0.000001;
        string loc1("NonLoc"), loc2("NonLoc"), loc3("NonLoc");
        maxSim = -999;
        for(it=fingerprint.begin(); it!=fingerprint.end(); it++){
                if((curSim = SimKeyCompare(it->second, test_finger)) > maxSim && abs(curSim) > epsilon){
                        maxSim = curSim;
                        loc1 = it->first;
                }
        }
        if(loc1.compare("NonLoc") != 0)
                closestLoc.push_back(loc1);
        else return;

        maxSim = -999;
        for(it=fingerprint.begin(); it!=fingerprint.end(); it++){
                if((curSim = SimKeyCompare(it->second, test_finger)) > maxSim && abs(curSim) > epsilon){
                        if(it->first != loc1){
                                maxSim = curSim;
                                loc2 = it->first;
                        }
                }
        }
        if(loc2.compare("NonLoc") != 0)
                closestLoc.push_back(loc2);
        else return;

        maxSim = -999;
        for(it=fingerprint.begin(); it!=fingerprint.end(); it++){
                if((curSim = SimKeyCompare(it->second, test_finger)) > maxSim && abs(curSim) > epsilon){
                        if(it->first != loc1 && it->first != loc2){
                                maxSim = curSim;
                               loc3 = it->first;
                        }
                }
        }
        if(loc3.compare("NonLoc") != 0)
                closestLoc.push_back(loc3);
}


//Determine which floor it is by comparing the number of 2K locations and 3K locations
void FloorCleaning(vector<string> & closestLoc, int & floor){
	int numFloorTwo=0, numFloorThree=0;
	vector<string>::iterator vec_it;
	for(vec_it = closestLoc.begin(); vec_it != closestLoc.end(); ++vec_it){
		if((*vec_it).find("2K") == string::npos) numFloorThree ++;
		else numFloorTwo ++;
	}
	if(numFloorThree > numFloorTwo){
		floor = 3;
		for(vec_it=closestLoc.begin(); vec_it!=closestLoc.end();){
			if((*vec_it).find("2K") != string::npos){
				vec_it = closestLoc.erase(vec_it);
			}else{
				++ vec_it;
			}
		}
	}	
	if(numFloorThree < numFloorTwo){
		floor = 2;
		for(vec_it=closestLoc.begin(); vec_it!=closestLoc.end();){
			if((*vec_it).find("3K") != string::npos){
				vec_it = closestLoc.erase(vec_it);
			}else{
				++ vec_it;
			}
		}
	}	
	if(numFloorThree == numFloorTwo) {
		if(numFloorThree == 0){ floor = 0; cout << "No Similar locations, error\n"; }
		else {floor = 0; cout << "Cannot detect floor number \n";}
	}
}

//Cleaning out those far away locations, assuming closestLoc has at most 3 elements. An optimization here.
void ClusterCleaning(map<string, pair<int, int> > & locMap, vector<string> & closestLoc){
	if(closestLoc.size() <= 2) return;
	//Now the case is only 3 elements
	string str1=closestLoc[0], str2=closestLoc[1], str3=closestLoc[2];
	pair<int, int> coor1=locMap[str1], coor2=locMap[str2], coor3=locMap[str3];
	double dis12=DisSquareTwoPts(coor1.first, coor1.second, coor2.first, coor2.second);
	double dis13=DisSquareTwoPts(coor1.first, coor1.second, coor3.first, coor3.second);
	double dis23=DisSquareTwoPts(coor2.first, coor2.second, coor3.first, coor3.second);
	double threshhold = numDoors*numDoors*doorDisSquare;
	if(dis12>threshhold && dis13>threshhold){ closestLoc.erase(closestLoc.begin()); return;}
	if(dis12>threshhold && dis23>threshhold){ closestLoc.erase(closestLoc.begin()+1); return;}	
	if(dis13>threshhold && dis23>threshhold){ closestLoc.erase(closestLoc.begin()+2); return;}
	return;
}

//Given the closest locations, compute the centroid, if necessary, project it to the corridor
double BestPt(map<string, pair<int, int> > & locMap, vector<string> & closestLoc, int & x, int & y){
	int cur_x=0, cur_y=0;
	vector<string>::iterator it;
	int num = closestLoc.size();
  if (num==0) { x=0, y=0; return 0; }
	for(it=closestLoc.begin(); it!=closestLoc.end(); ++it){
		pair<int, int> temp = locMap[*it];
		cur_x += temp.first;
		cur_y += temp.second;
	}
	cur_x /= num;
	cur_y /= num;
	if(num <= 3) { x = cur_x; y = cur_y; return 0;}
	//else actually return the distance between centroid and the projection on cooridor
	else return FindPointInCorridor(cur_x, cur_y, x, y);
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

void computeMedian(map<string, vector<int> > & test_raw_data, vector<pair<int, string> > & test_sigmac)
{
        map<string, vector<int> >::iterator it;
        for(it=test_raw_data.begin(); it!=test_raw_data.end(); ++it){
                sort(it->second.begin(), it->second.end());
                int median = *(it->second.begin() + it->second.size()/2);
                test_sigmac.push_back(pair<int, string> (median, it->first));
        }
}

int main(int argc, const char * argv[])
{
	if(argc != 4){
    /* some_median.csv - building fingerprints file
     * test_data_median.csv - location fingerprint
     * locations.csv  - the file which stores all the coordinates for each location
     */
		cout << "Usage: " << argv[0] << "  some_median.csv" << "  test_data_median.gatech-01.csv "<< " locations.csv" << endl;
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
        map<string, vector<int> > test_raw_data;
        map<string, int> test_fingerprint;
        while(getline(test_data, line)){
                stringstream lineStream(line);
                string macAdd;
                string sigStr;
                getline(lineStream, macAdd, ',');
                getline(lineStream, sigStr);
                test_raw_data[macAdd].push_back(atoi(sigStr.c_str()));
        }

        computeMedian(test_raw_data, test_sigmac);


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
	PickStrongFingerprint(test_sigmac, test_fingerprint, 10, 1);
	vector<string> closestLoc;
	SimKeyComparison(fingerprint, test_fingerprint, closestLoc);
	int floor;
	FloorCleaning(closestLoc, floor);
	ClusterCleaning(locMap, closestLoc); 
	
	vector<string>::iterator vec_it;
	for(vec_it=closestLoc.begin(); vec_it!=closestLoc.end(); vec_it++)
		cout << *vec_it << "  ";
	cout << endl;
	

	int x, y;
	char buffer[20];
	double dis = BestPt(locMap, closestLoc, x, y);
	cout << floor << ',' << x << ',' << y <<endl;

	//code added by nishith
	sprintf(buffer,"%d,%d,%d",floor,x,y);
        cout << buffer;
	FILE *fb;
	fb = fopen("loc","w");
	fwrite(buffer,strlen(buffer)*sizeof(char),1,fb);
	fclose(fb);
		
	return 0;
}	

