#include "mining_truck.h"

#include <cassert>

#include "unload_station.h"

namespace helium3 {

MiningTruck::MiningTruck(int32_t id)
    : id_(id)
    , state_(TruckState::Mining)
    , next_event_time_(0.0)
    , state_entered_at_(0.0) {	
}

int32_t MiningTruck::id() const { return id_; }
TruckState MiningTruck::state() const { return state_; }
double MiningTruck::next_event_time() const { return next_event_time_; }
UnloadStation* MiningTruck::assigned_station() const { return assigned_station_; }

double MiningTruck::start_mining(double current_time, double mining_duration) {
    // Accumulate time from previous state (TravelingToSite)
    // On simulation start state_ == Mining already, no accumulation needed
    if (state_ == TruckState::TravelingToSite) {
        total_travel_min_ += current_time - state_entered_at_;
    }

    state_            = TruckState::Mining;
    state_entered_at_ = current_time;
    next_event_time_  = current_time + mining_duration;
    assigned_station_ = nullptr;
    return next_event_time_;
}

double MiningTruck::start_travel_to_station(double current_time) {
    assert(state_ == TruckState::Mining);
    total_mining_min_ += current_time - state_entered_at_;

    state_            = TruckState::TravelingToStation;
    state_entered_at_ = current_time;
    next_event_time_  = current_time + TRAVEL_TIME_MIN;
    return next_event_time_;
}

void MiningTruck::join_queue(
    UnloadStation* station,
    double arrival_time,
    double unload_start_time
) {
    assert(state_ == TruckState::TravelingToStation);
    total_travel_min_ += arrival_time - state_entered_at_;

    assigned_station_ = station;
    state_            = TruckState::WaitingInQueue;
    state_entered_at_ = arrival_time;
    next_event_time_  = unload_start_time;
}

double MiningTruck::start_unloading(double current_time) {
    assert(state_ == TruckState::WaitingInQueue);
    total_queue_min_ += current_time - state_entered_at_;

    state_            = TruckState::Unloading;
    state_entered_at_ = current_time;
    next_event_time_  = current_time + UNLOAD_TIME_MIN;
    return next_event_time_;
}

double MiningTruck::start_travel_to_site(double current_time) {
    assert(state_ == TruckState::Unloading);
    total_unload_min_ += current_time - state_entered_at_;
    ++trips_completed_;

    assigned_station_ = nullptr;
    state_            = TruckState::TravelingToSite;
    state_entered_at_ = current_time;
    next_event_time_  = current_time + TRAVEL_TIME_MIN;
    return next_event_time_;
}

void MiningTruck::finalise(double now) {
    // Accumulate whatever partial period is in progress at simulation end.
    double elapsed = now - state_entered_at_;
    switch (state_) {
        case TruckState::Mining:             total_mining_min_ += elapsed; break;
        case TruckState::TravelingToStation: total_travel_min_ += elapsed; break;
        case TruckState::WaitingInQueue:     total_queue_min_  += elapsed; break;
        case TruckState::Unloading:          total_unload_min_ += elapsed; break;
        case TruckState::TravelingToSite:    total_travel_min_ += elapsed; break;
    }
}

} // namespace helium3
