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

class Ship;
class EntityManager;
class UI;

class Station : public std::enable_shared_from_this<Station>
{
public:
    Station(vec2f position, std::string_view name, std::shared_ptr<EntityManager> entityManager, std::shared_ptr<UI> ui, SDL_Renderer *renderer, TTF_Font *font);
    ~Station();

    virtual void tick(float dt) = 0;

    void addShip(std::shared_ptr<Ship> ship);
    void removeShip(int ship_id);

    void acceptTrade(wares::TradeType type, Ware ware, int quantity);

    void setMaintenanceLevel(Ware ware, int level);
    void reevaluateTradeOffers();

    void transferWares(std::shared_ptr<Ship> ship, Ware ware, int quantity);

    void requestDock(std::shared_ptr<Ship> ship);
    void undock(std::shared_ptr<Ship> ship);

    bool checkForAndHandleMouseClick(vec2f camera, Sint32 x, Sint32 y);
    void deselect();

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

protected:
    virtual void postUpdateInventory() = 0;

    int id;
    std::string name;

    bool m_Selected = false;

    std::shared_ptr<EntityManager> m_Manager;

    float credits;

    vec2f m_Position;

    std::shared_ptr<EntityManager> entityManager;
    std::shared_ptr<UI> m_UI;

    std::map<Ware, wares::Offer> sellOffers;
    std::map<Ware, wares::Offer> buyOffers;

    std::map<Ware, int> maintenanceLevels;

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

    void updateInventory(Ware ware, int quantity);

    void updateUI();

    // SDL
public:
    void render(vec2f camera);

protected:
    SDL_Renderer *m_Renderer;
    SDL_Texture *m_Texture;
    SDL_Texture *m_NameTexture;
    int m_NameTextWidth, m_NameTextHeight;
};
