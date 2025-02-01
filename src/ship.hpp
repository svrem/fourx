#pragma once

#include "vec.hpp"
#include "station.hpp"
#include "wares.hpp"
#include "orders.hpp"

#include <SDL2/SDL.h>

#include <memory>
#include <vector>
#include <map>
#include <optional>

using wares::Ware;

class Station;
class EntityManager;

class Ship : public std::enable_shared_from_this<Ship>
{
public:
    Ship(vec2f m_Position, float maxSpeed, float cargoCapacity, float weaponAttack, SDL_Renderer *renderer);

    void claim(std::shared_ptr<Station> station);
    void dock(std::shared_ptr<Station> station);

    void setManager(std::shared_ptr<EntityManager> manager);

    void searchForTrade(const std::vector<std::shared_ptr<Station>> &stations, float dt);

    void addWare(Ware ware, int quantity);

    void addOrder(ShipOrder order);
    void executeNextOrder();

    // NOOOO
    void intercept(std::shared_ptr<Ship> target, float dt);

    void doDamage(float damage);
    // {
    // hullHealth -= damage;

    // if (hullHealth <= 0)
    // {
    //     printf("Ship %d destroyed\n", id);
    //     this->m_Manager->removeShip(this->shared_from_this());
    // }
    // }

    const int getId() const
    {
        return id;
    }

    const int getCargoSpace() const
    {
        return cargoCapacity;
    }

    const vec2f getPosition() const
    {
        return m_Position;
    }

    const float getDirection() const
    {
        return m_CurrentDirection;
    }

    const float getHullHealth() const
    {
        return hullHealth;
    }

private:
    int id;

    std::shared_ptr<Station> owner = nullptr;
    std::shared_ptr<Station> dockedStation = nullptr;
    std::shared_ptr<Station> targetStation = nullptr;

    std::vector<ShipOrder> m_Orders;
    std::shared_ptr<EntityManager> m_Manager;

    vec2f m_Position;
    float m_CurrentDirection;
    std::optional<vec2f> m_Target;

    SDL_Renderer *m_Renderer;

    const float maxSpeed;
    const int cargoCapacity;
    const float weaponAttack;

    float m_TimeUntilNextTradeCheck = 0.0f;

    float hullHealth = 100.0f;

    std::map<Ware, int> m_Cargo;

    void undock();

    void setTarget(vec2f target);
    void setTarget(std::shared_ptr<Station> station);

    void attack(std::shared_ptr<Ship> target);

public:
    void render(vec2f camera, float zoomLevel, vec2f zoomCenter);
    void tick(float dt);
};

namespace ShipPreset
{
    inline std::shared_ptr<Ship> createFreighter(vec2f position, SDL_Renderer *renderer)
    {
        return std::make_shared<Ship>(position, 100, 1000, 0.1, renderer);
    }
}