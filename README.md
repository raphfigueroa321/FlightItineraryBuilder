# Flight Itinerary Builder

A C++ application that finds optimal flight itineraries between airports using a breadth-first search algorithm with customizable constraints.

## Implementation

This system efficiently plans multi-leg flight journeys by:
- Loading flight databases and airport information
- Finding valid flight paths between source and destination airports
- Respecting time constraints (connection times, layovers, total duration)
- Filtering by preferred airlines
- Validating and displaying complete itineraries

## Data File Formats

### Flight Database (`all_flights.txt`)
CSV format with the following columns:
```
Airline,FlightNumber,SourceAirport,DestinationAirport,DepartureTime,ArrivalTime,DurationSeconds
```

**Example:**
```csv
Delta,101,LAX,JFK,1736284800,1736306400,21600
United,202,JFK,ORD,1736320000,1736327200,7200
American,303,ORD,MIA,1736330000,1736340000,10000
Southwest,404,LAX,ORD,1736285000,1736299200,14200
```

**Field Descriptions:**
- `Airline`: Carrier name (e.g., "Delta", "United")
- `FlightNumber`: Numeric flight identifier
- `SourceAirport`: 3-letter IATA departure airport code
- `DestinationAirport`: 3-letter IATA arrival airport code
- `DepartureTime`: Unix timestamp (seconds since Jan 1, 1970 UTC)
- `ArrivalTime`: Unix timestamp (not used in calculation)
- `DurationSeconds`: Flight duration in seconds

### Airport Database (`airports.txt`)
CSV format with airport codes and coordinates:
```
AirportCode,Latitude,Longitude
```

**Example:**
```csv
LAX,33.9416,-118.4085
JFK,40.6413,-73.7781
ORD,41.9742,-87.9073
MIA,25.7959,-80.2870
```

### Test Parameters File (`param.txt`)
Text file with the following lines:

```
Line 1: Flight database filename
Line 2: Source airport code (3 letters)
Line 3: Destination airport code (3 letters)
Line 4: Start time (Unix timestamp in seconds)
Line 5: Maximum travel duration (hours)
Line 6: Minimum connection time (hours)
Line 7: Maximum layover time (hours)
Line 8+: Preferred airlines (optional, one per line)
```

**Example (`param.txt`):**
```
all_flights.txt
LAX
JFK
1736284800
24
1
12
Delta
United
```

## How to Run

### Prerequisites
- C++14 compatible compiler (Visual Studio 2015 or later, GCC 5+, Clang 3.4+)
- Flight data files (`all_flights.txt`, `airports.txt`)

### Compilation

**Using Visual Studio:**
1. Open `FlightItineraryBuilder.sln`
2. Build the solution (Ctrl+Shift+B)
3. Executable will be in `Debug/` or `Release/` folder

**Using g++:**
```bash
g++ -std=c++14 -o FlightItineraryBuilder main.cpp tp.cpp fm.cpp provided.cpp
```

### Running the Program

**Option 1: Command-line argument**
```bash
FlightItineraryBuilder.exe param.txt
```

**Option 2: Interactive mode**
```bash
FlightItineraryBuilder.exe
Enter test parameters file name: param.txt
```

### Example Output

**Successful Itinerary:**
```
Source: LAX, Destination: JFK, Total Duration: 8.5 hours
Arriving at source airport at: 2025-01-07 12:00 UTC (1736284800)
Wait time at initial airport: 1.33 hours
Flights:
  LAX -> ORD, Airline: Delta, Departure: 2025-01-07 13:20 UTC (1736289600), Arrival: 2025-01-07 17:16 UTC (1736303760), Duration: 3.93 hours
  Layover: 2.0 hours
  ORD -> JFK, Airline: United, Departure: 2025-01-07 19:16 UTC (1736311020), Arrival: 2025-01-07 21:16 UTC (1736318220), Duration: 2.0 hours
```

**No Valid Itinerary:**
```
No itineraries found matching your criteria.
```

**Invalid Itinerary:**
```
Invalid itinerary: Connection time between flights is less than minimum allowed
```