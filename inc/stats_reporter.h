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

        /**
         * @brief Format minutes as hours and minutes (ex: "2h 03m").
         *
         * @param minutes Value in minutes to format.
         * @return Formatted string representing hours and minutes.
         */
        std::string fmt_minutes(double minutes);

        /** @brief Print a horizontal line to out. */
        void hline(std::ostream& out, int width = DEFAULT_WIDTH);

        /** @brief Print a section header with surrounding separator lines. */
        void section_header(std::ostream& out, const std::string& title);
    } // namespace reporter_helpers

    namespace stats_reporter {
        /**
         * @brief Print a complete, human-readable simulation report.
         *
         * @param out Output stream to print the report to (ex: std::cout).
         * @param cfg Simulation configuration used for the run.
         * @param truck_stats Per-truck aggregated statistics.
         * @param station_stats Per-station aggregated statistics.
         * @param simulation_min Total simulation duration in minutes.
         */
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