#ifndef HELIUM3_MINING_TRUCK_H
#define HELIUM3_MINING_TRUCK_H

#include "sim_config.h"

namespace helium3 {

// Forward declaration
class UnloadStation;

class MiningTruck {
public:
    explicit MiningTruck(int id);

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
    int32_t id();
    TruckState state();
    double next_event_time();
    UnloadStation* assigned_station();

    // Finalise partial-period stats at simulation end
    void finalise(double now);

private:
    int32_t id_;
    TruckState state_               = TruckState::Mining;
    double nextEventTime_           = 0.0;
    UnloadStation* assignedStation_ = nullptr;
};

} // namespace helium3

#endif // HELIUM3_MINING_TRUCK_H