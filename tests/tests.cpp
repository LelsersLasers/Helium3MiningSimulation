#include <iostream>

#include "mining_truck.h"
#include "unload_station.h"

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

	// State transitions
	{
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

	// Stats
    {
        helium3::MiningTruck truck(2);
        truck.start_mining(0.0, 60.0); // mine 60 min
        truck.start_travel_to_station(60.0); // travel 30 min, arrives t=90
        truck.join_queue(nullptr, 90.0, 90.0);
        truck.start_unloading(90.0); // unload 5 min,  done t=95
        truck.start_travel_to_site(95.0); // travel 30 min, arrives t=125

        // Finalise at t=125, still travelling
        truck.finalise(125.0);

        auto s = truck.stats(125.0);
        test::check(test::near(s.total_mining_min,  60.0), "Mining time accumulated");
        test::check(test::near(s.total_travel_min, 30.0 + 30.0),
                    "Travel time accumulated (both legs)");
        test::check(test::near(s.total_queue_min,   0.0),  "Queue time = 0 (no wait)");
        test::check(test::near(s.total_unload_min,  5.0),  "Unload time accumulated");
    }
}

void test_unload_station() {
	test::beginSuite("UnloadStation");

    // Idle station,truck served immediately
    {
        helium3::UnloadStation station(1);
        test::check(!station.is_busy(), "Station starts idle");
        test::check(test::near(station.projected_wait_time(0.0), 0.0),
                    "projectedWaitTime = 0 when idle");

        helium3::MiningTruck truck(1);
        // Get truck to WaitingInQueue
        truck.start_mining(0.0, 30.0);
        truck.start_travel_to_station(30.0);

        double startTime = station.enqueue_truck(&truck, 60.0);
        test::check(test::near(startTime, 60.0),
                    "Idle station: unload starts immediately");
        test::check(station.is_busy(), "Station is busy after enqueue");
        test::check(test::near(station.free_at(), 60.0 + helium3::UNLOAD_TIME_MIN),
                    "free_at = enqueueTime + UNLOAD_TIME_MIN");
    }

    // Queue, second truck waits
    {
        helium3::UnloadStation station(2);
        helium3::MiningTruck t1(1);
		helium3::MiningTruck t2(2);

        // Put trucks into TravelingToStation
        t1.start_mining(0.0, 30.0);
		t1.start_travel_to_station(30.0);
        t2.start_mining(0.0, 30.0);
		t2.start_travel_to_station(30.0);

        double s1 = station.enqueue_truck(&t1, 60.0);
        double s2 = station.enqueue_truck(&t2, 61.0);

        test::check(test::near(s1, 60.0), "First truck starts at arrival time");
        test::check(test::near(s2, 60.0 + helium3::UNLOAD_TIME_MIN),
                    "Second truck starts after first finishes");
    }

    // Completing a truck advances queue
    {
        helium3::UnloadStation station(3);
        helium3::MiningTruck t1(1);
		helium3::MiningTruck t2(2);

        t1.start_mining(0.0, 30.0);
		t1.start_travel_to_station(30.0);
        t2.start_mining(0.0, 30.0);
		t2.start_travel_to_station(30.0);

        station.enqueue_truck(&t1, 60.0);
        station.enqueue_truck(&t2, 60.0);

        // Complete t1
        helium3::MiningTruck* next = station.complete_current_unload(65.0);
        test::check(next == &t2, "completeCurrentUnload returns next queued truck");
        test::check(station.is_busy(), "Station still busy after first unload");

        // Complete t2
        helium3::MiningTruck* empty = station.complete_current_unload(70.0);
        test::check(empty == nullptr, "Station idle after last truck departs");
        test::check(!station.is_busy(), "Station not busy when queue empty");
    }

	// Stats
	{
        helium3::UnloadStation station(4);
        helium3::MiningTruck t1(1);
        t1.start_mining(0.0, 30.0); t1.start_travel_to_station(30.0);

        station.enqueue_truck(&t1, 60.0); // busy 60–65
        station.complete_current_unload(65.0);
        station.finalise(100.0);

        auto s = station.stats(100.0);
        test::check(s.trucks_serviced == 1,          "trucksServiced = 1");
        test::check(test::near(s.total_busy_min, 5.0),"totalBusyMin = UNLOAD_TIME_MIN");
        // Idle period: 0–60 and 65–100 = 95 min
        test::check(test::near(s.total_idle_min, 95.0),"totalIdleMin = 95 min");
        test::check(test::near(s.utilisation, 5.0),  "utilisation = 5% (5/100)");
    }
}
int main() {
    std::cout << "Helium-3 Mining Simulation Tests\n";

	test_mining_truck();
	test_unload_station();

	test::print_summary();

	bool all_passed = test::all_passed();
	return all_passed ? 0 : 1;
}