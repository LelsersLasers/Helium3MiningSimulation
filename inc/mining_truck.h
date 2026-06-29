#ifndef HELIUM3_MINING_TRUCK_H
#define HELIUM3_MINING_TRUCK_H

#include <cstddef>
#include <cstdint>

#include "sim_config.h"

namespace helium3 {

// Forward declaration
class UnloadStation;

/**
 * @brief Aggregated statistics for a single mining truck.
 *
 * Populated after simulation completion and used for reporting.
 */
struct MiningTruckStats {
    /** Truck identifier used for reporting */
    int32_t id = 0;

    /** Number of complete trips (site -> station -> site) completed. */
    size_t trips_completed = 0;

    /** Total minutes spent mining. */
    double total_mining_min = 0.0;

    /** Total minutes spent travelling (to station and back to site). */
    double total_travel_min = 0.0;

    /** Total minutes spent waiting in station queues. */
    double total_queue_min = 0.0;

    /** Total minutes spent unloading at stations. */
    double total_unload_min = 0.0;

    /** Mining efficiency as percentage of simulation duration. */
    double mining_efficiency = 0.0;
};

/**
 * @brief Represents a single mining truck and its state.
 *
 * Encapsulates the truck state-machine and accumulates per-truck
 * statistics used by the reporting system.
 */
class MiningTruck {
public:
    explicit MiningTruck(int32_t id);

    // State transitions

    /**
     * @brief Begin a mining period.
     *
     * @param current_time Current simulation time in minutes.
     * @param mining_duration Duration (minutes) the truck will mine.
     * @return Simulation time when mining will finish.
     */
    double start_mining(double current_time, double mining_duration);

    /**
     * @brief Begin traveling from a mining site to an unload station.
     *
     * @param current_time Current simulation time in minutes.
     * @return Arrival time at the unload station.
     */
    double start_travel_to_station(double current_time);

    /**
     * @brief Record that the truck has joined a station queue.
     *
     * @param station Pointer to the UnloadStation the truck selected.
     * @param arrivalTime Simulation time when the truck arrived at the station.
     * @param unloadStartTime Scheduled time when the truck will begin unloading.
     */
    void join_queue(UnloadStation* station, double arrivalTime, double unloadStartTime);

    /**
     * @brief Begin unloading the truck at its assigned station.
     *
     * @param current_time Current simulation time in minutes (start of unload).
     * @return Simulation time when unloading will finish.
     */
    double start_unloading(double current_time);

    /**
     * @brief Begin traveling back to a mining site after unloading.
     *
     * @param current_time Current simulation time in minutes.
     * @return Arrival time at the mining site.
     */
    double start_travel_to_site(double current_time);

    // Accessors
    /** @brief Get the truck identifier. */
    int32_t id() const;

    /** @brief Get the current state of the truck. */
    TruckState state() const;

    /** @brief Get the next scheduled event time for the truck. */
    double next_event_time() const;

    /** @brief Get the station currently assigned to the truck (or nullptr if
    * currently none assigned). */
    UnloadStation* assigned_station() const;

    /**
     * @brief Accumulate any partial-period statistics at simulation end.
     *
     * @param now Simulation end time in minutes.
     */
    void finalise(double now);

    /**
     * @brief Return aggregated statistics for this truck.
     *
     * @param simulation_duration Total simulation duration in minutes.
     * @return MiningTruckStats populated for reporting.
     */
    MiningTruckStats stats(double simulation_duration) const;

private:
    // State
    int32_t id_;
    TruckState state_                = TruckState::Mining;
    double next_event_time_          = 0.0;
    UnloadStation* assigned_station_ = nullptr;
    double state_entered_at_         = 0.0;

    // Time and stats tracking
    double total_mining_min_  = 0.0;
    double total_travel_min_  = 0.0;
    double total_queue_min_   = 0.0;
    double total_unload_min_  = 0.0;
    size_t trips_completed_   = 0;
};

} // namespace helium3

#endif // HELIUM3_MINING_TRUCK_H