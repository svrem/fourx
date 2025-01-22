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

class Ship : public std::enable_shared_from_this<Ship>
{
public:
    Ship(vec2f m_Position, float max_speed, float cargoCapacity, SDL_Renderer *renderer);

    void claim(std::shared_ptr<Station> station);
    void dock(std::shared_ptr<Station> station);

    void searchForTrade(const std::vector<std::shared_ptr<Station>> &stations);

    void addWare(Ware ware, int quantity);

    const int getId() const
    {
        return id;
    }

    const int getCargoSpace() const
    {
        return cargoCapacity;
    }

private:
    int id;

    std::shared_ptr<Station> owner = nullptr;
    std::shared_ptr<Station> dockedStation = nullptr;
    std::shared_ptr<Station> targetStation = nullptr;

    std::vector<ShipOrder> m_Orders;

    vec2f m_Position;
    std::optional<vec2f> m_Target;

    SDL_Renderer *m_Renderer;

    const float maxSpeed;
    const float cargoCapacity;

    std::map<Ware, int> m_Cargo;

    void undock();

    void setTarget(vec2f target);
    void setTarget(std::shared_ptr<Station> station);

    void addOrder(ShipOrder order);
    void executeNextOrder();

public:
    void render(vec2f camera);
    void tick(float dt);
};