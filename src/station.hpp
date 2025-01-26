#pragma once

#include "vec.hpp"
#include "utils.hpp"
#include "wares.hpp"
#include "ship.hpp"

// SDL
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

// std
#include <map>
#include <vector>
#include <string>
#include <memory>
#include <stdexcept>
#include <variant>

using wares::Ware;

struct ProductionModule
{
    std::vector<wares::WareQuantity> inputWares;
    std::vector<std::variant<wares::WareQuantity, wares::ShipOrder>> outputWares;

    bool halted = true;

    int cycle_time;
    float current_cycle_time = 0;
};

class Ship;
class EntityManager;

class Station : public std::enable_shared_from_this<Station>
{
public:
    Station(vec2f position, std::string_view name, std::shared_ptr<EntityManager> entityManager, SDL_Renderer *renderer, TTF_Font *font);

    void addShip(std::shared_ptr<Ship> ship);
    void removeShip(int ship_id);

    void addProductionModule(ProductionModule module);

    void acceptTrade(wares::TradeType type, Ware ware, int quantity);

    void setMaintenanceLevel(Ware ware, int level);
    void reevaluateTradeOffers();

    void transferWares(std::shared_ptr<Ship> ship, Ware ware, int quantity);

    void requestDock(std::shared_ptr<Ship> ship);
    void undock(std::shared_ptr<Ship> ship);

    void tick(float dt);

    int getId() const
    {
        return id;
    }

    const std::map<Ware, wares::Offer> &getBuyOffers() const
    {
        return buyOffers;
    }

    const std::map<Ware, wares::Offer> &getSellOffers() const
    {
        return sellOffers;
    }

    const vec2f &getPosition() const
    {
        return m_Position;
    }

    void __debug_print_inventory() const;

private:
    int id;
    std::string_view name;

    float credits;

    vec2f m_Position;

    std::shared_ptr<EntityManager> entityManager;

    std::map<Ware, wares::Offer> sellOffers;
    std::map<Ware, wares::Offer> buyOffers;

    std::map<Ware, int> maintenanceLevels;

    std::vector<ProductionModule> productionModules;

    std::map<Ware, int> inventory;
    // Virtual inventory keeping track of the wares that the station is planning to buy
    std::map<Ware, int> buyReservations;
    // Virtual inventory keeping track of the wares that the station is planning to sell
    std::map<Ware, int>
        sellReservations;

    const int m_max_docked_ships = 5;

    std::vector<std::shared_ptr<Ship>> owned_ships;
    std::vector<std::shared_ptr<Ship>> docked_ships;

    std::vector<std::shared_ptr<Ship>> dock_queue;

    void updateTradeOffer(wares::TradeType type, wares::Ware ware, int quantity, float priceChangePercentage);
    void startNewProductionCycle(ProductionModule &productionModule);

    void updateInventory(Ware ware, int quantity);

    // SDL
public:
    void render(vec2f camera);

private:
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    SDL_Texture *nameTexture;
    int nameTextWidth, nameTextHeight;
};