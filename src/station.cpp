#include "station.hpp"

#include <iostream>
#include <cassert>
#include <set>

Station::Station(vec2f position) : position(position)
{
    id = utils::generateId();
}

void Station::addShip(std::unique_ptr<Ship> ship)
{
    ship->claim(shared_from_this());
    ships.push_back(std::move(ship));
    printf("Ship added to station %d with ID=%d, current fleet size %d.\n", id, ships.back()->id, ships.size());
}

void Station::removeShip(int ship_id)
{
    for (int i = 0; i < ships.size(); i++)
    {

        if (ships[i]->id == ship_id)
        {
            ships.erase(ships.begin() + i);
            return;
        }
    }

    throw std::runtime_error("Ship not found");
}

void Station::addProductionModule(ProductionModule module)
{
    productionModules.push_back(module);
}

void Station::addInventory(Ware ware, int quantity)
{
    inventory[ware] += quantity;

    for (auto &productionModule : this->productionModules)
    {
        // if production module is halted, check if we can start a new cycle
        if (productionModule.halted)
            startNewProductionCycle(productionModule);
    }

    this->reevaluateTradeOffers();
}

void Station::__debug_print_inventory()
{
    std::cout << "===========================================\n";
    std::cout << "Inventory for station " << id << "\n\n";
    for (auto &item : inventory)
    {
        auto details = wares::wareDetails.at(item.first);
        std::cout << "Ware: " << details.name << "; Quantity: " << item.second << "\n";
    }
    std::cout << "\n===========================================" << std::endl;
}

void Station::removeInventory(Ware ware, int quantity)
{
    assert(inventory[ware] >= quantity);
    inventory[ware] -= quantity;
}

void Station::startNewProductionCycle(ProductionModule &productionModule)
{
    productionModule.halted = false;
    for (auto &inputWare : productionModule.inputWares)
    {
        if (inventory[inputWare.ware] < inputWare.quantity)
        {
            productionModule.halted = true;
            break;
        }
    }

    if (productionModule.halted)
    {
        // not enough input wares
        productionModule.current_cycle_time = 0;
        return;
    }

    for (auto &inputWare : productionModule.inputWares)
    {
        inventory[inputWare.ware] -= inputWare.quantity;
    }
}

void Station::reevaluateTradeOffers()
{
    for (auto const &[ware, level] : inventory)
    {
        int maintenanceLevelDiff = level - maintenanceLevels.at(ware);
        TradeType type = maintenanceLevelDiff > 0 ? TradeType::Sell : TradeType::Buy;
        float quantity = maintenanceLevelDiff > 0 ? maintenanceLevelDiff : -maintenanceLevelDiff;
        updateTradeOffer(type, ware, quantity);
    };
}

void Station::tick(float dt)
{
    for (auto &productionModule : this->productionModules)
    {
        if (productionModule.halted)
            continue;

        productionModule.current_cycle_time += dt;

        if (productionModule.current_cycle_time < productionModule.cycle_time)
            continue;

        for (auto &outputWare : productionModule.outputWares)
        {
            inventory[outputWare.ware] += outputWare.quantity;
        }

        productionModule.current_cycle_time -= productionModule.cycle_time;

        // start new cycle
        startNewProductionCycle(productionModule);
    }
}

void Station::updateTradeOffer(TradeType type, Ware ware, float quantity)
{
    if (type == TradeType::Sell)
    {
        float price = this->sellOffers.find(ware) != this->sellOffers.end() ? this->sellOffers[ware].price : wares::wareDetails.at(ware).max_price;

        sellOffers[ware] = {price, quantity};
        buyOffers.erase(ware);

        std::cout << "Station " << id << " updated sell offer for " << wares::wareDetails.at(ware).name << " to " << price << " credits per unit. For " << quantity << " units." << std::endl;

        return;
    }

    float price = this->buyOffers.find(ware) != this->buyOffers.end() ? this->buyOffers[ware].price : wares::wareDetails.at(ware).min_price;

    buyOffers[ware] = {price, quantity};
    sellOffers.erase(ware);

    std::cout << "Station " << id << " updated buy offer for " << wares::wareDetails.at(ware).name << " to " << price << " credits per unit. For " << quantity << " units." << std::endl;
}

void Station::setMaintenanceLevel(Ware ware, int level)
{
    maintenanceLevels[ware] = level;
}