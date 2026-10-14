#pragma once

#include <string>
#include <cstdint>
#include <cmath>

namespace Slic3r::GUI {

struct ResinPrinterProfile {
    std::string printer_name = "Generic MSLA Printer";

    double build_x_mm = 192.0;
    double build_y_mm = 120.0;
    double build_z_mm = 200.0;

    uint32_t resolution_x = 3840;
    uint32_t resolution_y = 2400;

    double layer_height_mm = 0.05;
    double normal_exposure_s = 2.5;
    double bottom_exposure_s = 30.0;
    uint32_t bottom_layers = 5;

    double lift_distance_mm = 6.0;
    double lift_speed_mm_min = 60.0;
    double retract_speed_mm_min = 150.0;

    bool validate(std::string& error) const {
        error.clear();

        if (printer_name.empty()) {
            error = "Printer name is empty";
            return false;
        }

        if (!std::isfinite(build_x_mm) || build_x_mm <= 0 ||
            !std::isfinite(build_y_mm) || build_y_mm <= 0 ||
            !std::isfinite(build_z_mm) || build_z_mm <= 0) {
            error = "Invalid build volume";
            return false;
        }

        if (resolution_x == 0 || resolution_y == 0) {
            error = "Invalid LCD resolution";
            return false;
        }

        if (!std::isfinite(layer_height_mm) ||
            layer_height_mm <= 0 ||
            layer_height_mm > build_z_mm) {
            error = "Invalid layer height";
            return false;
        }

        if (!std::isfinite(normal_exposure_s) ||
            normal_exposure_s <= 0 ||
            !std::isfinite(bottom_exposure_s) ||
            bottom_exposure_s <= 0) {
            error = "Invalid exposure time";
            return false;
        }

        if (!std::isfinite(lift_distance_mm) ||
            lift_distance_mm <= 0 ||
            !std::isfinite(lift_speed_mm_min) ||
            lift_speed_mm_min <= 0 ||
            !std::isfinite(retract_speed_mm_min) ||
            retract_speed_mm_min <= 0) {
            error = "Invalid movement settings";
            return false;
        }

        return true;
    }

    double pixel_size_x_mm() const {
        return resolution_x ? build_x_mm / resolution_x : 0.0;
    }

    double pixel_size_y_mm() const {
        return resolution_y ? build_y_mm / resolution_y : 0.0;
    }
};

}
