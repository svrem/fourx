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
}

void Station::__debug_print_inventory()
{
    std::cout << "===========================================\n";
    std::cout << "Inventory for station " << id << "\n\n";
    for (auto &item : inventory)
    {
        auto details = ware_details.at(item.first);
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

    std::cout << "Starting new production cycle for module with cycle time " << productionModule.cycle_time << std::endl;

    for (auto &inputWare : productionModule.inputWares)
    {
        inventory[inputWare.ware] -= inputWare.quantity;
    }
}

void Station::reevaluateTradeOffers()
{
    std::set<Ware> productionNecessities;
    for (auto &productionModule : this->productionModules)
    {
        for (auto &inputWare : productionModule.inputWares)
        {
            productionNecessities.insert(inputWare.ware);
        }
    }
};

void Station::tick(float dt)
{
    std::cout << "Station tick with dt=" << dt << std::endl;
    for (auto &productionModule : this->productionModules)
    {
        if (productionModule.halted)
            continue;

        std::cout << "Production module current cycle time: " << productionModule.current_cycle_time << std::endl;
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