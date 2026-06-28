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


void test_sample_lib() {
	test::beginSuite("Sample Library Tests");
}

int main() {
    std::cout << "Lunar Helium-3 Mining Simulation Tests\n";

	test_sample_lib();

	test::print_summary();

	bool all_passed = test::all_passed();
	return all_passed ? 0 : 1;
}