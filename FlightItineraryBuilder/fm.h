#ifndef FM_INCLUDED
#define FM_INCLUDED

#include "provided.h"
#include "bstset.h"
using namespace std;

class FlightManager : public FlightManagerBase {
public:
	FlightManager() {

	}
	~FlightManager() {

	}
	bool load_flight_data(string filename);
	vector<FlightSegment> find_flights(string source_airport, int start_time, int end_time) const;
private:
	vector<FlightSegment> m_flights;
    unordered_map<string, BSTSet<FlightSegment>> m_airportToFlights;
};





#endif //FM_INCLUDED