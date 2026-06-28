#include <iostream>

#include "sample_lib.h"

namespace test {

	size_t passed = 0;
	size_t failed = 0;

	void check(bool condition) {
		if (condition) {
			++passed;
		} else {
			++failed;
		}
	}

}

void test_sample_lib() {
	test::check(sample_lib::add(2, 3) == 5);
	test::check(sample_lib::add(-1, 1) == 0);
	test::check(sample_lib::add(0, 0) == 0);
}

int main() {
    std::cout << "Lunar Helium-3 Mining Simulation Tests\n";

	test_sample_lib();

	std::cout << "Tests passed: " << test::passed << "\n";
	std::cout << "Tests failed: " << test::failed << "\n";

	return 0;
}