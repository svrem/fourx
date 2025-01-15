#pragma once

#include "vec.hpp"
#include "utils.hpp"
#include "wares.hpp"
#include "ship.hpp"

// std
#include <map>
#include <vector>
#include <string>
#include <memory>
#include <stdexcept>

struct Offer
{
    Ware ware;
    float price;
    float quantity;
};

struct WareQuantity
{
    Ware ware;
    int quantity;
};

struct ProductionModule
{
    std::vector<WareQuantity> inputWares;
    std::vector<WareQuantity> outputWares;

    bool halted = true;

    int cycle_time;
    int current_cycle_time = 0;
};

class Ship;

class Station : public std::enable_shared_from_this<Station>
{
public:
    int id;

    Station(vec2f position);

    void addShip(std::unique_ptr<Ship> ship);
    void removeShip(int ship_id);

    void addProductionModule(ProductionModule module);

    // TODO: make this private and only allow adding inventory through production modules and/or trade
    void addInventory(Ware ware, int quantity);
    void removeInventory(Ware ware, int quantity);

    void tick(float dt);

    void __debug_print_inventory();

private:
    float credits;

    vec2f position;

    std::vector<Offer> sellOffers;
    std::vector<Offer> buyOffers;

    std::vector<ProductionModule> productionModules;

    std::map<Ware, int> inventory;

    std::vector<std::unique_ptr<Ship>> ships;

    void startNewProductionCycle(ProductionModule &productionModule);
    void reevaluateTradeOffers();
};