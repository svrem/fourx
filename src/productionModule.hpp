#pragma once

#include "wares.hpp"
#include "utils.hpp"

#include <vector>
#include <variant>

struct ProductionModule
{
    std::vector<wares::WareQuantity> inputWares;
    std::vector<std::variant<wares::WareQuantity, wares::ShipOrder>> outputWares;

    bool halted = true;

    int cycle_time;
    float current_cycle_time = 0;
};

namespace ProductionModulePreset
{
    inline ProductionModule createSiliconProduction()
    {
        ProductionModule siliconProduction;
        siliconProduction.outputWares.push_back(wares::WareQuantity{wares::Ware::Silicon, 150});
        siliconProduction.cycle_time = 5;
        // siliconProduction.current_cycle_time = utils::gen() % siliconProduction.cycle_time;
        return siliconProduction;
    }

    inline ProductionModule createSiliconWaferProduction()
    {
        struct ProductionModule siliconWaferProduction = {};
        siliconWaferProduction.inputWares.push_back({wares::Ware::Silicon, 100});
        siliconWaferProduction.outputWares.push_back(wares::WareQuantity{wares::Ware::SiliconWafers, 50});
        siliconWaferProduction.cycle_time = 5;
        // siliconWaferProduction.current_cycle_time = utils::gen() % siliconWaferProduction.cycle_time;
        return siliconWaferProduction;
    }
}