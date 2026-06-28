#include <cstdint>
#include <iostream>
#include <cstring>


#include "sim_config.h"


static void printUsage(const char* program_name) {
    std::cout <<
        "Usage: " << program_name << " [options]\n"
        "\n"
        "  -n <trucks>     Number of mining trucks       (default: 10)\n"
        "  -m <stations>   Number of unload stations     (default: 2)\n"
        "  -s <seed>       RNG seed (don't set = random)\n"
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

    std::cout << "Simulation Configuration:\n";
    std::cout << "  Number of trucks: " << cfg.num_trucks << "\n";
    std::cout << "  Number of stations: " << cfg.num_stations << "\n";
    if (cfg.random_seed.has_value()) {
        std::cout << "  RNG seed: " << cfg.random_seed.value() << "\n";
    } else {
        std::cout << "  RNG seed: random\n";
    }

    return 0;
}
