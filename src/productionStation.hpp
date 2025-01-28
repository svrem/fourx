#pragma once

#include "station.hpp"

struct ProductionModule
{
    std::vector<wares::WareQuantity> inputWares;
    std::vector<std::variant<wares::WareQuantity, wares::ShipOrder>> outputWares;

    bool halted = true;

    int cycle_time;
    float current_cycle_time = 0;
};

class ProductionStation : public Station
{
public:
    using Station::Station;

    void tick(float dt) override;
    void addProductionModule(ProductionModule module);

private:
    std::vector<ProductionModule> productionModules;

    void postUpdateInventory() override;
    void startNewProductionCycle(ProductionModule &productionModule);
};