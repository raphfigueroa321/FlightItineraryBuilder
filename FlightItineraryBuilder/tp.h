#ifndef TP_INCLUDED
#define TP_INCLUDED

#include "provided.h"
#include "bstset.h"
#include <queue>
#include <set>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>
#include <stack>

using namespace std;

class TravelPlanner : public TravelPlannerBase {
public:
	TravelPlanner(const FlightManagerBase& flight_manager, const AirportDB& airport_db);
	~TravelPlanner() {}
	void add_preferred_airline(string airline);
	bool plan_travel(string source_airport, string destination_airport, int start_time, Itinerary& itinerary) const;

private:
	vector<string> prefAirline;
};


#endif //TP_INCLUDED
