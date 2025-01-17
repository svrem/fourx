#include "station.hpp"

#include <iostream>
#include <cassert>
#include <set>

Station::Station(vec2f position) : position(position)
{
    id = utils::generateId();
}

void Station::addShip(std::shared_ptr<Ship> ship)
{
    ship->claim(shared_from_this());
    ships.push_back(std::move(ship));
    printf("Ship added to station %d with ID=%d, current fleet size %d.\n", id, ships.back()->getId(), ships.size());
}

void Station::removeShip(int ship_id)
{
    for (int i = 0; i < ships.size(); i++)
    {

        if (ships[i]->getId() == ship_id)
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

void Station::updateInventory(Ware ware, int quantity)
{
    if (inventory.find(ware) == inventory.end())
    {
        inventory[ware] = 0;
    }
    inventory[ware] += quantity;

    assert(inventory[ware] >= 0);

    for (auto &productionModule : this->productionModules)
    {
        // if production module is halted, check if we can start a new cycle
        if (productionModule.halted)
            startNewProductionCycle(productionModule);
    }

    this->reevaluateTradeOffers();
}

void Station::__debug_print_inventory() const
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
        this->updateInventory(inputWare.ware, -inputWare.quantity);
    }
}

// Reevaluates the trade offers for the station based on the current inventory levels
// and the reservations made by ships.
void Station::reevaluateTradeOffers()
{
    for (auto const &[ware, inventoryLevel] : inventory)
    {
        if (maintenanceLevels.find(ware) == maintenanceLevels.end())
        {
            // warning
            std::cerr << "Warning: Maintenance level for ware " << wares::wareDetails.at(ware).name << " has not been set.\n";

            continue;
        }

        int level = inventoryLevel + buyReservations[ware] - sellReservations[ware];
        int maintenanceLevelDiff = level - maintenanceLevels.at(ware);
        wares::TradeType type = maintenanceLevelDiff > 0 ? wares::TradeType::Sell : wares::TradeType::Buy;
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
            this->updateInventory(outputWare.ware, outputWare.quantity);
        }

        productionModule.current_cycle_time -= productionModule.cycle_time;

        // start new cycle
        startNewProductionCycle(productionModule);
    }
}

void Station::updateTradeOffer(wares::TradeType type, Ware ware, float quantity)
{
    if (type == wares::TradeType::Sell)
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
    if (inventory.find(ware) == inventory.end())
    {
        inventory[ware] = 0;
    }
    maintenanceLevels[ware] = level;
}

// Accepts a trade offer for a specific ware, in this case, the TradeType should be of the offer
// that's being accepted (i.e. if the client is buying, the TradeType should be Sell, and vice versa)
// Throws an exception if the trade is invalid (e.g. not enough inventory to sell).
void Station::acceptTrade(wares::TradeType type, Ware ware, float quantity)
{
    if (type == wares::TradeType::Sell)
    {
        if (inventory[ware] < quantity)
            throw std::runtime_error("Not enough inventory to sell");

        printf("Selling %f units of %d\n", quantity, ware);

        sellReservations[ware] += quantity;
        updateInventory(ware, -quantity);
    }
    else
    {
        printf("Buying %f units of %d\n", quantity, ware);
        // buyReservations[ware] += quantity;
        updateInventory(ware, quantity);
        reevaluateTradeOffers();
    }
}