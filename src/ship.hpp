#pragma once

#include "vec.hpp"
#include "station.hpp"
#include "wares.hpp"

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
    Ship(vec2f position, float max_speed, float cargoCapacity);

    void claim(std::shared_ptr<Station> station);
    void dock(std::shared_ptr<Station> station);

    void searchForTrade(const std::vector<std::shared_ptr<Station>> &stations);

    void undock();

    void setTarget(vec2f target);
    void setTarget(std::shared_ptr<Station> station);

    int getId() const
    {
        return id;
    }

    bool getSearchingForTrade() const
    {
        return searchingForTrade;
    }

private:
    int id;

    std::shared_ptr<Station> owner = nullptr;
    std::shared_ptr<Station> dockedStation = nullptr;
    std::shared_ptr<Station> targetStation = nullptr;

    vec2f m_position;
    std::optional<vec2f> m_target;

    const float maxSpeed;
    const float cargoCapacity;

    std::map<Ware, int> cargo;

    bool searchingForTrade = false;

public:
    void render(SDL_Renderer *renderer);
    void tick(float dt);
};