#pragma once

#include "ResinPrinterProfile.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <filesystem>
#include <stdexcept>
#include <string>

namespace Slic3r::GUI {

class ResinProfileStorage {
public:
    static void save(
        const ResinPrinterProfile& p,
        const std::filesystem::path& path)
    {
        std::string error;
        if (!p.validate(error))
            throw std::runtime_error(error);

        nlohmann::json j = {
            {"schema_version", 1},
            {"printer_name", p.printer_name},
            {"build_x_mm", p.build_x_mm},
            {"build_y_mm", p.build_y_mm},
            {"build_z_mm", p.build_z_mm},
            {"resolution_x", p.resolution_x},
            {"resolution_y", p.resolution_y},
            {"layer_height_mm", p.layer_height_mm},
            {"normal_exposure_s", p.normal_exposure_s},
            {"bottom_exposure_s", p.bottom_exposure_s},
            {"bottom_layers", p.bottom_layers},
            {"lift_distance_mm", p.lift_distance_mm},
            {"lift_speed_mm_min", p.lift_speed_mm_min},
            {"retract_speed_mm_min", p.retract_speed_mm_min}
        };

        std::ofstream file(path, std::ios::binary);
        if (!file)
            throw std::runtime_error("Cannot open profile file");

        file << j.dump(4) << '\n';

        if (!file)
            throw std::runtime_error("Cannot write profile file");
    }

    static ResinPrinterProfile load(
        const std::filesystem::path& path)
    {
        std::ifstream file(path, std::ios::binary);
        if (!file)
            throw std::runtime_error("Cannot open profile file");

        nlohmann::json j;
        file >> j;

        if (j.at("schema_version").get<int>() != 1)
            throw std::runtime_error("Unsupported profile version");

        ResinPrinterProfile p;

        p.printer_name = j.at("printer_name").get<std::string>();
        p.build_x_mm = j.at("build_x_mm").get<double>();
        p.build_y_mm = j.at("build_y_mm").get<double>();
        p.build_z_mm = j.at("build_z_mm").get<double>();
        p.resolution_x = j.at("resolution_x").get<uint32_t>();
        p.resolution_y = j.at("resolution_y").get<uint32_t>();
        p.layer_height_mm = j.at("layer_height_mm").get<double>();
        p.normal_exposure_s = j.at("normal_exposure_s").get<double>();
        p.bottom_exposure_s = j.at("bottom_exposure_s").get<double>();
        p.bottom_layers = j.at("bottom_layers").get<uint32_t>();
        p.lift_distance_mm = j.at("lift_distance_mm").get<double>();
        p.lift_speed_mm_min = j.at("lift_speed_mm_min").get<double>();
        p.retract_speed_mm_min = j.at("retract_speed_mm_min").get<double>();

        std::string error;
        if (!p.validate(error))
            throw std::runtime_error(error);

        return p;
    }
};

}
