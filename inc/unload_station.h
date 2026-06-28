#ifndef HELIUM3_UNLOAD_STATION_H
#define HELIUM3_UNLOAD_STATION_H

#include <cstddef>
#include <cstdint>
#include <queue>

namespace helium3 {

// Forward declaration
class MiningTruck;

class UnloadStation {
public:
    explicit UnloadStation(int32_t id);

    // Return the time (from now) until an arriving truck would begin unloading,
	// assuming no additional trucks join this queue.
    double projected_wait_time(double current_time) const;

	// Add a truck to this station's queue at the given arrival time. If the
	// station is idle, the truck will begin unloading immediatel.
	// Returns the simulation time at which the truck will start unloading.
    double enqueue_truck(MiningTruck* truck, double arrival_time);

	// Called when the current unload finishes. Advances the queue and schedules
	// the next truck.
	// Returns a pointer to the next truck (now unloading), or nullptr if the queue
	// is empty.
    MiningTruck* complete_current_unload(double finish_time);

    // Accessors
    int32_t id() const;
    bool is_busy() const;
    double free_at() const;

    // Accumulate partial-period stats at simulation end
    void finalise(double now);

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