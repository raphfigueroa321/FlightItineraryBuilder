#include "tp.h"

TravelPlanner::TravelPlanner(const FlightManagerBase& flight_manager, const AirportDB& airport_db) : TravelPlannerBase(flight_manager, airport_db) {}

void TravelPlanner::add_preferred_airline(string airline) {
	prefAirline.push_back(airline);
}

double TravelPlanner::calculate_heuristic(const string& from_airport, const string& to_airport) const {
	// Heuristic: estimated time to reach destination based on great circle distance
	// Assumes average flight speed of 500 mph (typical cruising speed)
	const double AVERAGE_FLIGHT_SPEED_MPH = 500.0;
	const double SECONDS_PER_HOUR = 3600.0;

	double distance_miles;
	if (!get_airport_db().get_distance(from_airport, to_airport, distance_miles)) {
		return 0.0; // If we can't get distance, return 0 (admissible heuristic)
	}

	// Convert distance to estimated time in seconds
	double estimated_time = (distance_miles / AVERAGE_FLIGHT_SPEED_MPH) * SECONDS_PER_HOUR;
	return estimated_time;
}

bool TravelPlanner::plan_travel(string source_airport, string destination_airport, int start_time, Itinerary& itinerary) const {
	// Priority queue for A* algorithm (min-heap based on f_cost)
	priority_queue<PathNode, vector<PathNode>, ComparePathNode> openSet;

	// Track visited airports with their best arrival time to avoid redundant paths
	unordered_map<string, int> visited;

	bool isPreferredAirline;

	// Initialize with flights from source airport
	int minTime = start_time;
	int maxTime = start_time + get_max_layover();
	vector<FlightSegment> initialFlights = get_flight_manager().find_flights(source_airport, minTime, maxTime);

	// Add initial flights to open set
	for (int i = 0; i < initialFlights.size(); i++) {
		isPreferredAirline = prefAirline.empty() ||
			(prefAirline.end() != find(prefAirline.begin(), prefAirline.end(), initialFlights[i].airline));

		int arrivalTime = initialFlights[i].departure_time + initialFlights[i].duration_sec;
		int totalTime = arrivalTime - start_time;

		if (isPreferredAirline && totalTime < get_max_duration()) {
			vector<FlightSegment> newPath = { initialFlights[i] };

			// Check if this is a direct flight to destination
			if (initialFlights[i].destination_airport == destination_airport) {
				itinerary.flights = newPath;
				itinerary.source_airport = source_airport;
				itinerary.destination_airport = destination_airport;
				itinerary.total_duration = totalTime;
				return true;
			}

			// Calculate heuristic for this path
			double h_cost = calculate_heuristic(initialFlights[i].destination_airport, destination_airport);

			PathNode node(newPath, totalTime, h_cost, initialFlights[i].destination_airport, arrivalTime);
			openSet.push(node);
		}
	}

	// A* main loop
	while (!openSet.empty()) {
		PathNode currentNode = openSet.top();
		openSet.pop();

		string currentAirport = currentNode.current_airport;
		int currentTime = currentNode.current_time;

		// Check if we've reached destination
		if (currentAirport == destination_airport) {
			itinerary.flights = currentNode.flights;
			itinerary.source_airport = source_airport;
			itinerary.destination_airport = destination_airport;
			itinerary.total_duration = currentNode.g_cost;
			return true;
		}

		// Skip if we've already visited this airport with a better or equal time
		if (visited.count(currentAirport) > 0) {
			if (visited[currentAirport] <= currentTime) {
				continue; // Already found a better path to this airport
			}
		}
		visited[currentAirport] = currentTime;

		// Find connecting flights
		minTime = currentTime + get_min_connection_time();
		maxTime = currentTime + get_max_layover();
		vector<FlightSegment> nextFlights = get_flight_manager().find_flights(currentAirport, minTime, maxTime);

		// Explore neighbors
		for (int i = 0; i < nextFlights.size(); i++) {
			isPreferredAirline = prefAirline.empty() ||
				(prefAirline.end() != find(prefAirline.begin(), prefAirline.end(), nextFlights[i].airline));

			int nextArrivalTime = nextFlights[i].departure_time + nextFlights[i].duration_sec;
			int totalTime = nextArrivalTime - start_time;

			// Check constraints
			if (!isPreferredAirline || totalTime >= get_max_duration()) {
				continue;
			}

			// Check if airport already visited in this path (prevent cycles)
			bool alreadyInPath = false;
			for (const auto& flight : currentNode.flights) {
				if (flight.destination_airport == nextFlights[i].destination_airport) {
					alreadyInPath = true;
					break;
				}
			}

			if (alreadyInPath) {
				continue;
			}

			// Check if airported already visited w/ better time
			if (visited.count(nextFlights[i].destination_airport) > 0) {
				if (visited[nextFlights[i].destination_airport] <= nextArrivalTime) {
					continue;
				}
			}

			// Create new path node
			vector<FlightSegment> newPath = currentNode.flights;
			newPath.push_back(nextFlights[i]);

			double h_cost = calculate_heuristic(nextFlights[i].destination_airport, destination_airport);

			PathNode newNode(newPath, totalTime, h_cost, nextFlights[i].destination_airport, nextArrivalTime);
			openSet.push(newNode);
		}
	}

	return false; // No path found
}