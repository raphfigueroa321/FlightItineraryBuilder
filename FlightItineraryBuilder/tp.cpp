#include "tp.h"

TravelPlanner::TravelPlanner(const FlightManagerBase& flight_manager, const AirportDB& airport_db) : TravelPlannerBase(flight_manager, airport_db) {}

void TravelPlanner::add_preferred_airline(string airline) {
	prefAirline.push_back(airline);
}

bool TravelPlanner::plan_travel(string source_airport, string destination_airport, int start_time, Itinerary& itinerary) const {
	queue<vector<FlightSegment>> possiblePaths;

	int minTime = start_time;
	int maxTime = start_time + get_max_layover();
	vector<FlightSegment> initialFlights = get_flight_manager().find_flights(source_airport, minTime, maxTime);
	bool isPreferredAirline;

	//start with direct flights
	for (int i = 0; i < initialFlights.size(); i++) {
		isPreferredAirline = prefAirline.empty() ||
			(prefAirline.end() != find(prefAirline.begin(), prefAirline.end(), initialFlights[i].airline));

		if (isPreferredAirline && initialFlights[i].duration_sec < get_max_duration()) {
			vector<FlightSegment> newPath = { initialFlights[i] };
			possiblePaths.push(newPath);

			if (initialFlights[i].destination_airport == destination_airport) {
				itinerary.flights = newPath;
				itinerary.source_airport = source_airport;
				itinerary.destination_airport = destination_airport;
				itinerary.total_duration = initialFlights[i].departure_time + initialFlights[i].duration_sec - start_time;
				return true;
			}
		}
	}

	//check all possible flights
	while (!possiblePaths.empty()) {
		vector<FlightSegment> curPath = possiblePaths.front();
		possiblePaths.pop();

		FlightSegment lastFlight = curPath.back();
		string currentAirport = lastFlight.destination_airport;
		int currentArrival = lastFlight.departure_time + lastFlight.duration_sec;

		//process path
		if (currentAirport == destination_airport) {
			itinerary.flights = curPath;
			itinerary.source_airport = source_airport;
			itinerary.destination_airport = destination_airport;
			itinerary.total_duration = currentArrival - start_time;
			return true;
		}

		minTime = currentArrival + get_min_connection_time();
		maxTime = currentArrival + get_max_layover();

		vector<FlightSegment> nextFlights = get_flight_manager().find_flights(currentAirport, minTime, maxTime);

		int totalDuration;
		for (int i = 0; i < nextFlights.size(); i++) {
			isPreferredAirline = prefAirline.empty() ||
				(prefAirline.end() != find(prefAirline.begin(), prefAirline.end(), nextFlights[i].airline));

			totalDuration = (nextFlights[i].departure_time - start_time) + nextFlights[i].duration_sec;

			//check if airport already visited
			int duplicates = 0;
			for (int j = 0; j < curPath.size(); j++) {
				if (curPath[j].destination_airport == nextFlights[i].destination_airport) {
					duplicates++;
				}
			}

			if (isPreferredAirline && totalDuration < get_max_duration() && duplicates < 1) {
				vector<FlightSegment> newPath = curPath;
				newPath.push_back(nextFlights[i]);
				possiblePaths.push(newPath);
			}
		}
	}
	return false;
}