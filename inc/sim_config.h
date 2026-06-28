#ifndef HELIUM3_SIM_TYPES_H
#define HELIUM3_SIM_TYPES_H

#include <cstdint>

namespace helium3 {

// Simulation constants
inline constexpr double SIMULATION_DURATION_MIN = 72.0 * 60.0; // 72 hours
inline constexpr double TRAVEL_TIME_MIN         = 30.0;        // Between site and station
inline constexpr double UNLOAD_TIME_MIN         = 5.0;        // Per truck at station
inline constexpr double MIN_MINE_TIME_MIN       = 1.0 * 60.0; // 1 hour
inline constexpr double MAX_MINE_TIME_MIN       = 5.0 * 60.0; // 5 hours


} // namespace helium3


#endif // HELIUM3_SIM_TYPES_H
