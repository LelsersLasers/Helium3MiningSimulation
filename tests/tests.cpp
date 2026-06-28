#include <iostream>

#include "mining_truck.h"

namespace test {

	size_t passed = 0;
	size_t failed = 0;

	// Check a condition and print the result, updating passed/failed counts
	void check(bool condition, const std::string& label) {
		if (condition) {
			++passed;
			std::cout << "  [PASS] " << label << '\n';
		} else {
			++failed;
			std::cout << "  [FAIL] " << label << '\n';
		}
	}

	// Compare doubles with a tolerance
	// Return true if a and b are within tol of each other, does not update
	// passed/failed counts
	bool near(double a, double b, double tol = 0.001) {
		return std::abs(a - b) <= tol;
	}

	// Print name of test suite as a header
	void beginSuite(const std::string& name) {
		std::cout << "\n=== " << name << " ===\n";
	}

	// Print summary of passed/failed results
	void print_summary() {
		std::cout << "\n----------------------------------------\n";
		std::cout << "Results: " << passed << " passed, " << failed << " failed\n";
	}

	// Return true if all tests passed, false otherwise
	bool all_passed() {
		return failed == 0;
	}

} // namespace test


void test_mining_truck() {
    test::beginSuite("MiningTruck");

	// Initial state
	helium3::MiningTruck truck(1);
	test::check(truck.state() == helium3::TruckState::Mining,
				"Initial state is Mining");

	// Start mining for 120 minutes
	double done = truck.start_mining(0.0, 120.0);
	test::check(test::near(done, 120.0),
				"start_mining returns correct finish time");
	test::check(truck.state() == helium3::TruckState::Mining,
				"State remains Mining after start_mining");

	// Travel to station
	double arrival = truck.start_travel_to_station(120.0);
	test::check(test::near(arrival, 120.0 + helium3::TRAVEL_TIME_MIN),
				"Travel arrival time = miningDone + TRAVEL_TIME_MIN");
	test::check(truck.state() == helium3::TruckState::TravelingToStation,
				"State is TravelingToStation after start_travel_to_station");

	// Join queue
	truck.join_queue(nullptr, 150.0, 150.0);
	test::check(truck.state() == helium3::TruckState::WaitingInQueue,
				"State is WaitingInQueue after join_queue");

	// Start unloading
	double unloadDone = truck.start_unloading(150.0);
	test::check(test::near(unloadDone, 150.0 + helium3::UNLOAD_TIME_MIN),
				"Unload finish = start + UNLOAD_TIME_MIN");
	test::check(truck.state() == helium3::TruckState::Unloading,
				"State is Unloading after start_unloading");
	
	// Travel back to site
	double siteArrival = truck.start_travel_to_site(155.0);
	test::check(test::near(siteArrival, 155.0 + helium3::TRAVEL_TIME_MIN),
				"Site arrival = unloadDone + TRAVEL_TIME_MIN");
	test::check(truck.state() == helium3::TruckState::TravelingToSite,
				"State is TravelingToSite after start_travel_to_site");
}

int main() {
    std::cout << "Lunar Helium-3 Mining Simulation Tests\n";

	test_mining_truck();

	test::print_summary();

	bool all_passed = test::all_passed();
	return all_passed ? 0 : 1;
}