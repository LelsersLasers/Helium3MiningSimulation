#include "stats_reporter.h"
#include "mining_truck.h"
#include "unload_station.h"

#include <iomanip>
#include <algorithm>
#include <sstream>

namespace helium3 {

namespace reporter_helpers {

std::string fmt_min(double minutes) {
    int h = static_cast<int>(minutes) / 60;
    int m = static_cast<int>(minutes) % 60;
    std::ostringstream oss;
    oss << h << "h " << std::setw(2) << std::setfill('0') << m << "m";
    return oss.str();
}

// Horizontal separator line
void hline(std::ostream& out, int width) {
    out << std::string(width, '-') << '\n';
}

void section_header(std::ostream& out, const std::string& title) {
    hline(out);
    out << "  " << title << '\n';
    hline(out);
}

} // namespace reporter_helpers


namespace stats_reporter {

// Print a full formatted report.
void print(
	std::ostream& out,
	const SimConfig& cfg,
	const std::vector<MiningTruckStats>& truck_stats,
	const std::vector<UnloadStationStats>& station_stats,
	double simulation_min
) {
    const double sim_hours = simulation_min / 60.0;

    // Header
    out << '\n';
    out << std::string(reporter_helpers::DEFAULT_WIDTH, '=') << '\n';
    out << "  LUNAR HE-3 MINING SIMULATION — FINAL REPORT\n";
    out << std::string(reporter_helpers::DEFAULT_WIDTH, '=') << '\n';
    out << std::fixed << std::setprecision(1);
    out << "  Configuration : " << cfg.num_trucks   << " trucks, "
                                << cfg.num_stations << " unload station"
                                << (cfg.num_stations != 1 ? "s" : "") << '\n';
    out << "  Duration      : " << sim_hours << " hours  ("
                                << simulation_min << " minutes)\n";
    out << "  RNG seed      : "
        << (cfg.random_seed ? std::to_string(cfg.random_seed.value()) : "random") << '\n';
    out << '\n';

    // Per-truck
    reporter_helpers::section_header(out, "MINING TRUCK STATISTICS");

    out << std::left
        << std::setw(6)  << "Truck"
        << std::setw(8)  << "Trips"
        << std::setw(14) << "Mining"
        << std::setw(14) << "Travel"
        << std::setw(14) << "Queuing"
        << std::setw(14) << "Unloading"
        << std::setw(10) << "Mine %"
        << '\n';
    reporter_helpers::hline(out);

    double sum_mining = 0.0, sum_travel = 0.0, sum_queue = 0.0, sum_unload = 0.0;
    size_t sum_trips  = 0;

    for (const MiningTruckStats& s : truck_stats) {
        out << std::left
            << std::setw(6)  << ("#" + std::to_string(s.id))
            << std::setw(8)  << s.trips_completed
            << std::setw(14) << reporter_helpers::fmt_minutes(s.total_mining_min)
            << std::setw(14) << reporter_helpers::fmt_minutes(s.total_travel_min)
            << std::setw(14) << reporter_helpers::fmt_minutes(s.total_queue_min)
            << std::setw(14) << reporter_helpers::fmt_minutes(s.total_unload_min)
            << std::setprecision(1) << std::setw(10) << s.mining_efficiency
            << '\n';
        sum_mining += s.total_mining_min;
        sum_travel += s.total_travel_min;
        sum_queue  += s.total_queue_min;
        sum_unload += s.total_unload_min;
        sum_trips  += s.trips_completed;
    }

    reporter_helpers::hline(out);
    double total_truck_min = static_cast<double>(cfg.num_trucks) * simulation_min;
    out << std::left
        << std::setw(6)  << "TOTAL"
        << std::setw(8)  << sum_trips
        << std::setw(14) << reporter_helpers::fmt_minutes(sum_mining)
        << std::setw(14) << reporter_helpers::fmt_minutes(sum_travel)
        << std::setw(14) << reporter_helpers::fmt_minutes(sum_queue)
        << std::setw(14) << reporter_helpers::fmt_minutes(sum_unload)
        << std::setprecision(1)
        << std::setw(10) << (sum_mining / total_truck_min * 100.0)
        << '\n';

    // Per-station
    out << '\n';
    reporter_helpers::section_header(out, "UNLOAD STATION STATISTICS");

    out << std::left
        << std::setw(10) << "Station"
        << std::setw(10) << "Serviced"
        << std::setw(14) << "Busy"
        << std::setw(14) << "Idle"
        << std::setw(12) << "Util %"
        << std::setw(12) << "Max Queue"
        << std::setw(14) << "Avg Wait"
        << '\n';
    reporter_helpers::hline(out);

    double sum_busy = 0, sum_idle = 0, sum_wait = 0;
    size_t sum_serviced = 0, max_queue = 0;

    for (const UnloadStationStats& s : station_stats) {
        double avgWait = s.trucks_serviced > 0
                         ? s.total_queue_wait_min / s.trucks_serviced : 0.0;
        out << std::left
            << std::setw(10) << ("#" + std::to_string(s.id))
            << std::setw(10) << s.trucks_serviced
            << std::setw(14) << reporter_helpers::fmt_minutes(s.total_busy_min)
            << std::setw(14) << reporter_helpers::fmt_minutes(s.total_idle_min)
            << std::setprecision(1) << std::setw(12) << s.utilisation
            << std::setw(12) << s.max_queue_depth
            << std::setw(14) << reporter_helpers::fmt_minutes(avgWait)
            << '\n';
        sum_busy     += s.total_busy_min;
        sum_idle     += s.total_idle_min;
        sum_wait     += s.total_queue_wait_min;
        sum_serviced += s.trucks_serviced;
        max_queue     = std::max(max_queue, s.max_queue_depth);
    }

    reporter_helpers::hline(out);
    double total_station_min = static_cast<double>(cfg.num_stations) * simulation_min;
    double avg_wait_all = sum_serviced > 0 ? sum_wait / sum_serviced : 0.0;
    out << std::left
        << std::setw(10) << "TOTAL"
        << std::setw(10) << sum_serviced
        << std::setw(14) << reporter_helpers::fmt_minutes(sum_busy)
        << std::setw(14) << reporter_helpers::fmt_minutes(sum_idle)
        << std::setprecision(1) << std::setw(12) << (sum_busy / total_station_min * 100.0)
        << std::setw(12) << max_queue
        << std::setw(14) << reporter_helpers::fmt_minutes(avg_wait_all)
        << '\n';

    // Summary
    out << '\n';
    reporter_helpers::section_header(out, "SUMMARY");
    out << "  Total trips completed       : " << sum_trips    << '\n';
    out << "  Total time mining           : " << reporter_helpers::fmt_minutes(sum_mining)
                                              << "  ("
                                              << std::setprecision(1)
                                              << (sum_mining / total_truck_min * 100.0)
                                              << "% of fleet time)\n";
    out << "  Total time traveling        : " << reporter_helpers::fmt_minutes(sum_travel) << '\n';
    out << "  Total time queuing          : " << reporter_helpers::fmt_minutes(sum_queue)  << '\n';
    out << "  Total time unloading        : " << reporter_helpers::fmt_minutes(sum_unload) << '\n';
    out << "  Combined station utilisation: "
        << std::setprecision(1)
        << (sum_busy / total_station_min * 100.0) << "%\n";
    out << std::string(reporter_helpers::DEFAULT_WIDTH, '=') << '\n' << '\n';
}

} // namespace reporter

}  // namespace lunar
