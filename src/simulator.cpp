#include "simulator.h"

#include <cassert>
#include <iostream>

#include "sim_config.h"
#include "mining_truck.h"
#include "unload_station.h"
#include "stats_reporter.h"


namespace helium3 {

Simulator::Simulator(SimConfig cfg)
    : cfg_(cfg) {

    // Seed RNG
    if (cfg_.random_seed) {
        rng_.seed(cfg_.random_seed.value());
    } else {
        std::random_device rd;
        rng_.seed(rd());
    }

    // Trucks
    trucks_.reserve(cfg_.num_trucks);
    for (int i = 0; i < cfg_.num_trucks; ++i) {
        trucks_.emplace_back(std::make_unique<MiningTruck>(i + 1));
    }

    // Stations
    stations_.reserve(cfg_.num_stations);
    std::vector<UnloadStation*> station_ptrs;
    for (int i = 0; i < cfg_.num_stations; ++i) {
        stations_.emplace_back(std::make_unique<UnloadStation>(i + 1));
    }
}

// ---------------------------------------------------------------------------
void Simulator::run() {
    assert(!ran_ && "Simulator::run() called more than once");
    ran_ = true;

    // Initial events, all trucks start mining at t=0
    for (auto& t : trucks_) {
        double mine_for    = random_mining_duration();
        double finish_time = t->start_mining(0.0, mine_for);
        schedule_event(finish_time, t.get(), TruckState::TravelingToStation);
    }

    // Main simulation event loop
    while (!event_queue_.empty()) {
        Event ev = event_queue_.top();
        event_queue_.pop();

        // Discard events beyond the simulation end time
        if (ev.time > SIMULATION_DURATION_MIN) {
            break;
        }

        process_event(ev);
    }

    // Finalise stats
    for (auto& t : trucks_) {
        t->finalise(SIMULATION_DURATION_MIN);
    }
    for (auto& s : stations_) {
        s->finalise(SIMULATION_DURATION_MIN);
    }
}

// ---------------------------------------------------------------------------
void Simulator::process_event(const Event& ev) {
    MiningTruck* truck = ev.truck;
    double now         = ev.time;

    switch (ev.next_state) {
        case TruckState::TravelingToStation: handle_mining_complete(truck, now);    break;
        case TruckState::WaitingInQueue:     handle_arrival_at_station(truck, now); break;
        case TruckState::Unloading:          handle_start_unloading(truck, now);    break;
        case TruckState::TravelingToSite:    handle_unload_complete(truck, now);    break;
        case TruckState::Mining:             handle_arrival_at_site(truck, now);    break;
    }
}

// Mining finished, travel to station
void Simulator::handle_mining_complete(MiningTruck* truck, double now) {
    double arrival_time = truck->start_travel_to_station(now);
    schedule_event(arrival_time, truck, TruckState::WaitingInQueue);
}

void Simulator::handle_arrival_at_station(MiningTruck* truck, double now) {
    // Find the station with the shortest projected wait
    // Handle ties by picking the station with the lowest ID
    UnloadStation* best  = nullptr;
    double best_wait     = std::numeric_limits<double>::max();

    for (auto& s : stations_) {
        double w = s->projected_wait_time(now);
        bool lower_wait = w < best_wait;
        bool tie_break  = w == best_wait && (!best || s->id() < best->id());
        if (lower_wait || (w == best_wait && tie_break)) {
            best_wait = w;
            best      = s.get();
        }
    }

    // Enqueue truck; get back the time it will begin unloading
    double unload_start = best->enqueue_truck(truck, now);
    truck->join_queue(best, now, unload_start);

    // Schedule the unloading event
    schedule_event(unload_start, truck, TruckState::Unloading);
}

void Simulator::handle_start_unloading(MiningTruck* truck, double now) {
    double finish_time = truck->start_unloading(now);
    schedule_event(finish_time, truck, TruckState::TravelingToSite);
}

void Simulator::handle_unload_complete(MiningTruck* truck, double now) {
    // Update the station so it can advance its queue
    UnloadStation* station = truck->assigned_station();
    if (station) {
        station->complete_current_unload(now);
    }

    // Truck travels back to a mining site
    double arrival_time = truck->start_travel_to_site(now);
    schedule_event(arrival_time, truck, TruckState::Mining);
}

void Simulator::handle_arrival_at_site(MiningTruck* truck, double now) {
    double mine_for    = random_mining_duration();
    double finish_time = truck->start_mining(now, mine_for);
    schedule_event(finish_time, truck, TruckState::TravelingToStation);
}

void Simulator::schedule_event(double time, MiningTruck* truck, TruckState next) {
    if (time <= SIMULATION_DURATION_MIN) {
        event_queue_.push({time, truck, next});
    }
}

double Simulator::random_mining_duration() {
    return mining_dist_(rng_);
}

// ---------------------------------------------------------------------------
void Simulator::print_report() const {
    assert(ran_ && "Call run() before printReport()");

    std::vector<MiningTruckStats> ts;
    std::vector<UnloadStationStats> ss;
    ts.reserve(trucks_.size());
    ss.reserve(stations_.size());

    for (const auto& t : trucks_) {
        ts.push_back(t->stats(SIMULATION_DURATION_MIN));
    }
    for (const auto& s : stations_) {
        ss.push_back(s->stats(SIMULATION_DURATION_MIN));
    }

    stats_reporter::print(std::cout, cfg_, ts, ss, SIMULATION_DURATION_MIN);
}

} // namespace helium3
