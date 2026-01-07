#include "fm.h"

bool operator<(const FlightSegment& lhs, const FlightSegment& rhs) {
    return lhs.departure_time < rhs.departure_time;
}

bool operator>(const FlightSegment& lhs, const FlightSegment& rhs) {
    return lhs.departure_time > rhs.departure_time;
}

bool operator<=(const FlightSegment& lhs, const FlightSegment& rhs) {
    return lhs.departure_time <= rhs.departure_time;
}

bool FlightManager::load_flight_data(string filename) {

    std::ifstream file(filename);
    if (!file)
    {
        std::cerr << "Can't open flight database: " << filename << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(file, line))
    {
        std::istringstream ss(line);
        std::string airline, source_airport, destination_airport;
        std::string flight_no_str, departure_time_str, arrival_time_str, duration_sec_str;

        if (!std::getline(ss, airline, ',') ||
            !std::getline(ss, flight_no_str, ',') ||
            !std::getline(ss, source_airport, ',') ||
            !std::getline(ss, destination_airport, ',') ||
            !std::getline(ss, departure_time_str, ',') ||
            !std::getline(ss, arrival_time_str, ',') ||
            !std::getline(ss, duration_sec_str, ','))
            continue;

        int flight_no = std::stoi(flight_no_str);
        int departure_time = std::stoi(departure_time_str);
        int duration_sec = std::stoi(duration_sec_str);

        FlightSegment s(airline, flight_no, source_airport, destination_airport, departure_time, duration_sec);
        m_flights.push_back(s);

        m_airportToFlights[source_airport].insert(s);
    }
    return true;
}

vector<FlightSegment> FlightManager::find_flights(string source_airport, int start_time, int end_time) const {
    vector<FlightSegment> matchFlights;
    unordered_map<string, BSTSet<FlightSegment>>::const_iterator iter;
    iter = m_airportToFlights.find(source_airport);
    if (iter == m_airportToFlights.end()) {
        return matchFlights;
    }

    const BSTSet<FlightSegment>& possibleFlights = iter->second;
    typename BSTSet<FlightSegment>::SetIterator it = possibleFlights.find_first_not_smaller(FlightSegment("", 0, source_airport, "", start_time, 0));

    const FlightSegment* flight;

    // In-order traversal directly using the iterator to avoid O(log N) per flight
    while ((flight = it.get_and_advance()) != nullptr) {
        if (flight->departure_time > end_time) {
            break;  // We can stop if the flight departs after the end time
        }

        // Add flight if it fits the time window
        if (flight->departure_time >= start_time && flight->departure_time <= end_time) {
            matchFlights.push_back(*flight);
        }
    }

    return matchFlights;
}