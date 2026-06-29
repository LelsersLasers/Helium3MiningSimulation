#include <iostream>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <chrono>

#include "sim_config.h"
#include "simulator.h"


static void printUsage(const char* program_name) {
    std::cout <<
        "Usage: " << program_name << " [options]\n"
        "\n"
        "  -n <trucks>     Number of mining trucks       (default: 10)\n"
        "  -m <stations>   Number of unload stations     (default: 2)\n"
        "  -s <seed>       RNG seed                      (default: random)\n"
        "  -h              Show this help\n"
        "\n"
        "Example:\n"
        "  " << program_name << " -n 30 -m 3\n";
}

int main(int argc, char* argv[]) {
    helium3::SimConfig cfg;

    // Parse command-line arguments
    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "-h") == 0 ||
            std::strcmp(argv[i], "--help") == 0) {
            printUsage(argv[0]);
            return 0;
        }
        else if (std::strcmp(argv[i], "-n") == 0 && i + 1 < argc) {
            cfg.num_trucks = std::atoi(argv[++i]);
        }
        else if (std::strcmp(argv[i], "-m") == 0 && i + 1 < argc) {
            cfg.num_stations = std::atoi(argv[++i]);
        }
        else if (std::strcmp(argv[i], "-s") == 0 && i + 1 < argc) {
            cfg.random_seed = static_cast<uint32_t>(std::atoi(argv[++i]));
        }
        else {
            std::cerr << "Unknown argument or missing value: " << argv[i] << "\n\n";
            printUsage(argv[0]);
            return 1;
        }
    }

    // Validate configuration values
    if (cfg.num_trucks < 1) {
        std::cerr << "Error: number of trucks must be >= 1\n";
        return 1;
    }
    if (cfg.num_stations < 1) {
        std::cerr << "Error: number of stations must be >= 1\n";
        return 1;
    }

    // Run the simulation
    std::cout << "Lunar He-3 Mining Simulation starting...\n";
    std::cout << "Trucks: " << cfg.num_trucks
              << "  Stations: " << cfg.num_stations;
    if (cfg.random_seed) {
        std::cout << "  Seed: " << cfg.random_seed.value();
    } else {
        std::cout << "  Seed: random";
    }
    std::cout << "\n\n";

    auto wall_start = std::chrono::high_resolution_clock::now();

    helium3::Simulator sim(cfg);
    sim.run();

    auto wall_end = std::chrono::high_resolution_clock::now();
    double wall_ms = std::chrono::duration<double, std::milli>(wall_end - wall_start).count();

    sim.print_report();

    std::cout << "Wall-clock execution time: " << wall_ms << " ms\n\n";
    return 0;
}
