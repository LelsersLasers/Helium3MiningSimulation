#include "unload_station.h"

#include <cassert>

#include "mining_truck.h"


namespace helium3 {

UnloadStation::UnloadStation(int id)
    : id_(id)
    , free_at_(0.0)
    , idle_start_(0.0) {
}

int32_t UnloadStation::id() const { return id_; }
bool UnloadStation::is_busy() const { return current_truck_ != nullptr; }
double UnloadStation::free_at() const { return free_at_; }

double UnloadStation::projected_wait_time(double currentTime) const {
    return std::max(0.0, free_at_ - currentTime);
}

double UnloadStation::enqueue_truck(MiningTruck* truck, double arrival_time) {
    double unload_start;

    if (!is_busy()) {
        // Station idle -> serve immediately
        total_idle_min_ += arrival_time - idle_start_;
        unload_start    = arrival_time;
        current_truck_  = truck;
        start_unloading(unload_start);
    } else {
        // Else -> queue behind existing trucks
        unload_start = free_at_;
        wait_queue_.push({truck, unload_start});
        free_at_ += UNLOAD_TIME_MIN;

        max_queue_depth_ = std::max(max_queue_depth_, wait_queue_.size());
    }

    // Accumulate for stats
    double wait = unload_start - arrival_time;
    total_queue_wait_min_ += wait;

    return unload_start;
}

void UnloadStation::start_unloading(double start_time) {
    // trucksServiced_ is counted in complete_current_unload so trucks mid unload
    // at sim-end are not counted
    free_at_ = start_time + UNLOAD_TIME_MIN;
    total_busy_min_   += UNLOAD_TIME_MIN;
}

MiningTruck* UnloadStation::complete_current_unload(double finish_time) {
    assert(current_truck_ != nullptr);
    ++trucks_serviced_;
    current_truck_ = nullptr;

    if (wait_queue_.empty()) {
        idle_start_ = finish_time;
        return nullptr;
    }

    auto [next_truck, scheduled_start] = wait_queue_.front();
    wait_queue_.pop();
    current_truck_ = next_truck;

    start_unloading(finish_time);
    return next_truck;
}

void UnloadStation::finalise(double now) {
    if (!is_busy()) {
        total_idle_min_ += now - idle_start_;
    }
    // If a truck is mid-unload at sim end, its partial busy time was already
    // added by start_unloading()
}

UnloadStationStats UnloadStation::stats(double simulation_duration) const {
    UnloadStationStats s;
    s.id                   = id_;
    s.trucks_serviced      = trucks_serviced_;
    s.total_busy_min       = total_busy_min_;
    s.total_idle_min       = total_idle_min_;
    s.utilisation          = (simulation_duration > 0.0)
                             ? (total_busy_min_ / simulation_duration) * 100.0
                             : 0.0;
    s.max_queue_depth      = max_queue_depth_;
    s.total_queue_wait_min = total_queue_wait_min_;
    return s;
}

} // namespace helium3
