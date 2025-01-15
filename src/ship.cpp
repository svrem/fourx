#include "ship.hpp"

Ship::Ship(vec2f position, float max_speed, float cargo_capacity) : position(position), max_speed(max_speed), cargo_capacity(cargo_capacity)
{
    this->id = utils::generateId();
}

void Ship::claim(std::shared_ptr<Station> station)
{
    this->owner = station;
}

void Ship::dock(std::shared_ptr<Station> station)
{
    this->docked_station = station;
}