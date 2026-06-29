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

/**
 * @brief Internal event representation used by the event queue.
 *
 * Events schedule state transitions for trucks at given simulation times.
 * Implements comparison operator for use in a priority queue.
 */
struct Event {
    /** Scheduled simulation time for the event (minutes). */
    double time  = 0.0;

    /** Truck that the event concerns. */
    MiningTruck* truck = nullptr;

    /** State the truck will transition to when the event triggers. */
    TruckState next_state = TruckState::Mining;

    /**
     * @brief Comparator used by the priority queue.
     *
     * Returns true if this event is later than o (so the priority
     * queue orders smallest time first).
     */
    bool operator>(const Event& o) const;
};

/**
 * @brief Discrete-event simulator for the mining operation.
 *
 * Intendd use: construct with a SimConfig, call run() exactly once to execute
 * the simulation, then use print_report() to display results.
 */
class Simulator {
public:
    explicit Simulator(SimConfig cfg);
    ~Simulator() = default;

    // Non-copyable, owns unique_ptrs to trucks and stations
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
    /** @brief Access the collection of trucks (for tests). */
    const std::vector<std::unique_ptr<MiningTruck>>& trucks() const;

    /** @brief Access the collection of stations (for tests). */
    const std::vector<std::unique_ptr<UnloadStation>>& stations() const;

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
