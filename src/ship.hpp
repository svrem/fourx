#pragma once

#include "vec.hpp"
#include "station.hpp"
#include "wares.hpp"

#include <memory>
#include <vector>
#include <map>

using wares::Ware;

class Station;

class Ship
{
public:
    Ship(vec2f position, float max_speed, float cargoCapacity);

    void claim(std::shared_ptr<Station> station);
    void dock(std::shared_ptr<Station> station);

    void searchForTrade(const std::vector<std::shared_ptr<Station>> &stations);

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

    std::shared_ptr<Station> owner;
    std::shared_ptr<Station> docked_station;

    vec2f position;
    const float maxSpeed;
    const float cargoCapacity;

    std::map<Ware, int> cargo;

    bool searchingForTrade = false;

public:
};