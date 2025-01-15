#pragma once

#include "vec.hpp"
#include "station.hpp"
#include "wares.hpp"

#include <memory>
#include <map>

class Station;

class Ship
{
public:
    int id;

    void claim(std::shared_ptr<Station> station);
    void dock(std::shared_ptr<Station> station);

private:
    std::shared_ptr<Station> owner;
    std::shared_ptr<Station> docked_station;

    vec2f position;
    const float max_speed;
    const float cargo_capacity;

    std::map<Ware, int> cargo;

public:
    Ship(vec2f position, float max_speed, float cargo_capacity);
};