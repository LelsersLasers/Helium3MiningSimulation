#ifndef HELIUM3_STATS_REPORTER_H
#define HELIUM3_STATS_REPORTER_H

#include <ostream>
#include <vector>

#include "sim_config.h"
#include "mining_truck.h"
#include "unload_station.h"

namespace helium3 {

    namespace reporter_helpers {
        inline constexpr int DEFAULT_WIDTH = 80;

        std::string fmt_minutes(double minutes);
        void hline(std::ostream& out, int width = DEFAULT_WIDTH);
        void section_header(std::ostream& out, const std::string& title);
    } // namespace reporter_helpers

    namespace stats_reporter {
        // Print a full formatted report.
        void print(
            std::ostream& out,
            const SimConfig& cfg,
            const std::vector<MiningTruckStats>& truck_stats,
            const std::vector<UnloadStationStats>& station_stats,
            double simulation_min
        );
    } // namespace reporter

} // namespace helium3

#endif // HELIUM3_STATS_REPORTER_H