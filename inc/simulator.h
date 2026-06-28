#ifndef HELIUM3_SIMULATOR_H
#define HELIUM3_SIMULATOR_H

#include <memory>
#include <queue>
#include <random>
#include <vector>

#include "sim_config.h"
#include "mining_truck.h"
#include "unload_station.h"


namespace helium3 {

// Internal event type
struct Event {
    double       time  = 0.0;
    MiningTruck* truck = nullptr;
    TruckState   next_state = TruckState::Mining;

    // Smallest time first; min-heap comparator
    bool operator>(const Event& o) const;
};

class Simulator {
public:
    explicit Simulator(SimConfig cfg);
    ~Simulator() = default;

    // Non-copyable
    Simulator(const Simulator&)            = delete;
    Simulator& operator=(const Simulator&) = delete;

    /**
     * @brief Run the full 72-hour simulation.
     *
     * Designed to be called exactly once per Simulator instance.
     */
    void run();

    /**
     * @brief Print a formatted report to standard output after run() completes.
     */
    void print_report() const;

    // Helpers/exposers for unit tests
    const std::vector<std::unique_ptr<MiningTruck>>&   trucks()   const { return trucks_; }
    const std::vector<std::unique_ptr<UnloadStation>>& stations() const { return stations_; }

private:
    // Helpers
    double random_mining_duration();
    void schedule_event(double time, MiningTruck* truck, TruckState next);
    void process_event(const Event& ev);

    // State transition handlers
    void handle_arrival_at_station(MiningTruck* truck, double now);
    void handle_start_unloading(MiningTruck* truck, double now);
    void handle_unload_complete(MiningTruck* truck, double now);
    void handle_arrival_at_site(MiningTruck* truck, double now);
    void handle_mining_complete(MiningTruck* truck, double now);
    
    // Simulation configuration
    SimConfig cfg_;

    // Simulation state
    std::vector<std::unique_ptr<MiningTruck>>   trucks_;
    std::vector<std::unique_ptr<UnloadStation>> stations_;

    // Smallest/earliest time at top (min-heap priority queue)
    using EventQueue = std::priority_queue<Event,
                                           std::vector<Event>,
                                           std::greater<Event>>;
    EventQueue event_queue_;

    // RNG
    std::mt19937                         rng_;
    std::uniform_real_distribution<double> mining_dist_{
        MIN_MINE_TIME_MIN, MAX_MINE_TIME_MIN};

    // Protection against double calling run()
    bool ran_ = false;
};

} // namespace helium3

#endif // HELIUM3_SIMULATOR_H
