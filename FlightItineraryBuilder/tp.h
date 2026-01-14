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
	
	// A* specific structures
	struct PathNode {
		vector<FlightSegment> flights;
		int g_cost;  // Actual cost: total time from start to current
		double f_cost;  // g_cost + h_cost (estimated total cost)
		string current_airport;
		int current_time;
		
		PathNode(const vector<FlightSegment>& f, int g, double h, const string& airport, int time)
			: flights(f), g_cost(g), f_cost(g + h), current_airport(airport), current_time(time) {}
	};
	
	struct ComparePathNode {
		bool operator()(const PathNode& a, const PathNode& b) const {
			// Lower f_cost has higher priority
			if (abs(a.f_cost - b.f_cost) > 0.01)
				return a.f_cost > b.f_cost;
			// If f_costs are equal, prefer fewer connections
			return a.flights.size() > b.flights.size();
		}
	};
	
	double calculate_heuristic(const string& from_airport, const string& to_airport) const;
};


#endif //TP_INCLUDED
