#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <utility>
#include <vector>
#include <map>
#include <algorithm>
#include <QPointf>
#include <QSqlQuery>
#include <QBitmap>

using namespace std;

const int MedianInterval = 20;
const int FrequencyRatioCutOff = 5;

/*
 * This function is intended to take a raw huge file, consolidate rows with same 
 * <location, Mac Address> field and compute the median. Save the output file as 
 * the all fingerprint which can be loaded in the future. 
*/

QList< QPair<QString, QPointF> > MainDialog::get_measurement_locations_from_db(Qlist present_capture)
{
  QSqlQuery q(m_db);
  QList< QPair<QString, QPointF> > rvalue;

  q.prepare(" \
      SELECT \
        capture_location_name, capture_location_x_pos, capture_location_y_pos \
      FROM capture_locations \
      ");

  //q.bindValue(":capture_location_floor", floor_number);

  bool res = q.exec();

  if (!res)
  {
    QMessageBox::warning(this, "Database Error",
        "Failed during SELECT from capture_locations table.");
    return rvalue;
  }

  while (q.next())
  {
    QPair<QString, QPointF> tuple;

    tuple.first = q.value(0).toString();
    tuple.second = QPointF(q.value(1).toFloat(), q.value(2).toFloat());

    rvalue.append(tuple);
  }

  calculate_positions_from_data(rvalue,present_capture);
}


int calculate_positions_from_data(QList< QPair<QString, QPointF> > data, QList present_capture)
{
	
	QList < QPair<Qstring,QPointF> >::iterator i;
	
	string line;
	//for each <location, MacAdd>, store all the signal strength in a vector
	map<string, map<string, vector<int> > > locMacSig; 
	map<string, map<string, vector<int> > >::iterator locMacSig_It;
	
	//store all the rows in ifm in a LocMacSig map structure
	for(i=data.begin() ; i!= data.end() ; ++i)
	{
		
		locMacSig[location][macAdd].push_back(i);
	}

	vector<int> medianVec, frequencyVec;

	locMacSig_It = locMacSig.begin();
	for(; locMacSig_It != locMacSig.end(); locMacSig_It++){
		//Now inside the loop, it's for a single location
		map<string, vector<int> > & macSig = locMacSig_It -> second;
		map<string, vector<int> >::iterator macSig_It;
		medianVec.clear();
		frequencyVec.clear();

		for(macSig_It = macSig.begin(); macSig_It != macSig.end(); macSig_It ++){
			vector<int> & sigVec = macSig_It->second;
			sort(sigVec.begin(), sigVec.end()); //sort the vector in order to compute median
			int median = *(sigVec.begin() + sigVec.size()/2);
			medianVec.push_back(median);
			frequencyVec.push_back(sigVec.size());
		}
		sort(medianVec.begin(), medianVec.end());
		sort(frequencyVec.begin(), frequencyVec.end());
 		int medianCutOff = medianVec.back()-MedianInterval;
		int frequencyCutOff = frequencyVec.back()/FrequencyRatioCutOff;

		for(macSig_It = macSig.begin(); macSig_It != macSig.end(); macSig_It ++){
			vector<int> & sigVec = macSig_It->second;
			int median = *(sigVec.begin() + sigVec.size()/2);
			if(median >= medianCutOff && sigVec.size() >= frequencyCutOff){
				ofm << locMacSig_It->first << ','
			    	<< macSig_It->first << ','
			    	<< median << endl;
			}
		}	
	
	}
	

	return 0;
}	
