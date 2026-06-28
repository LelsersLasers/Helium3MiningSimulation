#ifndef HELIUM3_SIM_TYPES_H
#define HELIUM3_SIM_TYPES_H

#include <cstddef>
#include <cstdint>

namespace helium3 {

// Simulation constants
inline constexpr double SIMULATION_DURATION_MIN = 72.0 * 60.0; // 72 hours
inline constexpr double TRAVEL_TIME_MIN         = 30.0;        // Between site and station
inline constexpr double UNLOAD_TIME_MIN         = 5.0;        // Per truck at station
inline constexpr double MIN_MINE_TIME_MIN       = 1.0 * 60.0; // 1 hour
inline constexpr double MAX_MINE_TIME_MIN       = 5.0 * 60.0; // 5 hours


// Truck state machine
enum class TruckState {
    Mining,             // Currently extracting Helium-3 at a site
    TravelingToStation, // Progressing to an unload station
    WaitingInQueue,     // In line at a station, not yet unloading
    Unloading,          // Currently unloading at a station
    TravelingToSite     // Returning to a mining site after unload
};

// Simulation configuration
struct SimConfig {
    size_t num_trucks;
    size_t num_stations;
    uint32_t random_seed = 42u; // < 0 = use random_device
};

} // namespace helium3


#endif // HELIUM3_SIM_TYPES_H
