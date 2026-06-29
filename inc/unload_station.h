#ifndef HELIUM3_UNLOAD_STATION_H
#define HELIUM3_UNLOAD_STATION_H

#include <cstddef>
#include <cstdint>
#include <queue>

namespace helium3 {

// Forward declaration
class MiningTruck;

/**
 * @brief Aggregated statistics for an unload station.
 *
 * Populated after simulation completion and used for reporting.
 */
struct UnloadStationStats {
    /** Station identifier used for reporting and tie-breaking. */
    int32_t id = 0;

    /** Number of trucks fully serviced by this station. */
    size_t trucks_serviced = 0;

    /** Total minutes station was busy unloading trucks. */
    double total_busy_min = 0.0;

    /** Total minutes station was idle. */
    double total_idle_min = 0.0;

    /** Station utilisation as percentage of the simulation duration. */
    double utilisation = 0.0;

    /** Maximum observed queue depth at this station. */
    size_t max_queue_depth = 0;

    /** Total minutes trucks spent waiting in this station's queue. */
    double total_queue_wait_min = 0.0;
};

/**
 * @brief Represents an unload station with a queue of trucks.
 *
 * Handles a FIFO queue of trucks, schedules unloading events and accumulates
 * station statistics used by the reporting system.
 */
class UnloadStation {
public:
    explicit UnloadStation(int32_t id);

    /**
     * @brief Get the projected wait time for an arriving truck.
     *
     * This returns the time (in minutes) from current_time until the
     * next available unloading slot, assuming no additional arrivals.
     *
     * @param current_time Current simulation time in minutes.
     * @return Minutes until an arriving truck would begin unloading.
     */
    double projected_wait_time(double current_time) const;

    /**
     * @brief Enqueue a truck at this station.
     *
     * If the station is idle the truck will start unloading immediately.
     * Otherwise it will be placed at the end of the wait queue.
     *
     * @param truck Pointer to the MiningTruck arriving.
     * @param arrival_time Simulation time when the truck arrives.
     * @return Scheduled simulation time when the truck will begin unloading.
     */
    double enqueue_truck(MiningTruck* truck, double arrival_time);

    /**
     * @brief Complete the current unload and advance the queue.
     *
     * Called when the currently unloading truck finishes. Updates internal
     * counters and, if a queued truck exists, starts its unloading.
     *
     * @param finish_time Simulation time when the unload finished.
     * @return Pointer to the truck which now begins unloading, or nullptr
     *         if the queue was empty.
     */
    MiningTruck* complete_current_unload(double finish_time);

    // Accessors
    /** @brief Station identifier. */
    int32_t id() const;

    /** @brief True if a truck is currently unloading. */
    bool is_busy() const;

    /** @brief Time (simulation minutes) when the station will next be free. */
    double free_at() const;

    /**
     * @brief Accumulate partial-period station stats at simulation end.
     *
     * @param now Simulation end time in minutes.
     */
    void finalise(double now);

    /**
     * @brief Return aggregated statistics for this station.
     *
     * @param simulation_duration Total simulation duration in minutes.
     * @return UnloadStationStats populated for reporting.
     */
    UnloadStationStats stats(double simulation_duration) const;

private:
    // Begin unloading the front of the queue
    void start_unloading(double start_time);

    int32_t id_;
    MiningTruck* current_truck_ = nullptr;
    double free_at_             = 0.0;

    // Queue of trucks waiting behind currentTruck_, stored as (truck*,
	// expected_start_time)
    std::queue<std::pair<MiningTruck*, double>> wait_queue_;

    // Stats accumulators
    size_t trucks_serviced_      = 0;
    double total_busy_min_       = 0.0;
    double idle_start_           = 0.0;
    double total_idle_min_       = 0.0;
    size_t max_queue_depth_      = 0;
    double total_queue_wait_min_ = 0.0;
};


} // namespace helium3

#endif // HELIUM3_UNLOAD_STATION_H