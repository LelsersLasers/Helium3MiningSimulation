#ifndef HELIUM3_MINING_TRUCK_H
#define HELIUM3_MINING_TRUCK_H

#include <cstddef>
#include <cstdint>

#include "sim_config.h"

namespace helium3 {

// Forward declaration
class UnloadStation;

struct MiningTruckStats {
    int32_t id               = 0;
    size_t trips_completed   = 0;
    double total_mining_min  = 0.0;
    double total_travel_min  = 0.0;
    double total_queue_min   = 0.0;
    double total_unload_min  = 0.0;
    double mining_efficiency = 0.0;
};

class MiningTruck {
public:
    explicit MiningTruck(int32_t id);

    // State transitions

    // Begin mining, returns the simulation time when mining will finish
    double start_mining(double current_time, double mining_duration);

    // Begin traveling to an unload station, returns arrival time
    double start_travel_to_station(double current_time);

    // Record that the truck joined a queue and will start unloading at start_time
    void join_queue(UnloadStation* station, double arrivalTime, double unloadStartTime);

    // Begin unloading, returns the simulation time when unloading will finish
    double start_unloading(double current_time);

    // Begin traveling back to a mining site, returns arrival time at site.
    double start_travel_to_site(double current_time);

    // Accessors
    int32_t id() const;
    TruckState state() const;
    double next_event_time() const;
    UnloadStation* assigned_station() const;

    // Accumulate partial-period stats at simulation end
    void finalise(double now);

    // Return statics for this truck, based on the simulation duration.
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