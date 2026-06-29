#ifndef HELIUM3_SIM_TYPES_H
#define HELIUM3_SIM_TYPES_H

#include <cstdint>
#include <optional>

namespace helium3 {

// Simulation constants

// Per handout, the simulated duration is 72 hours
inline constexpr double SIMULATION_DURATION_MIN = 72.0 * 60.0;
// Travel time (minutes) between a mining site and an unload station
inline constexpr double TRAVEL_TIME_MIN = 30.0;
// Unloading time (minutes) per truck at a station.
inline constexpr double UNLOAD_TIME_MIN = 5.0;
// Minimum mining duration (minutes) for a single mining period.
inline constexpr double MIN_MINE_TIME_MIN = 1.0 * 60.0;
// Maximum mining duration (minutes) for a single mining period.
inline constexpr double MAX_MINE_TIME_MIN = 5.0 * 60.0;


/**
 * @brief Truck behaviour state machine.
 *
 * Represents the discrete states a MiningTruck may be in during
 * the simulation.
 */
enum class TruckState {
    /// Currently extracting Helium-3 at a mining site
    Mining,

    /// Traveling from a mining site to an unload station
    TravelingToStation,

    /// Waiting in the station's queue (not yet unloading)
    WaitingInQueue,

    /// Currently unloading at a station
    Unloading,

    /// Traveling back to a mining site after unloading
    TravelingToSite
};

/**
 * @brief Top-level simulation configuration.
 *
 * Parameters that control the behaviour of a single simulation
 * run and are passed in via CLI.
 */
struct SimConfig {
    /** Number of mining trucks in the simulation (default: 10). */
    int32_t num_trucks                  = 10;

    /** Number of unload stations in the simulation (default: 2). */
    int32_t num_stations                = 2;

    /** Optional RNG seed. If std::nullopt, a random seed via
     * std::random_device is used. */
    std::optional<uint32_t> random_seed = std::nullopt;
};

} // namespace helium3


#endif // HELIUM3_SIM_TYPES_H
