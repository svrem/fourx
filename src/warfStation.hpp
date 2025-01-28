#pragma once

#include "station.hpp"

struct ShipConstructionOrder
{
    int ownerID;

    float maxSpeed;
    float cargoCapacity;
    float weaponAttack;
    float timeToConstruct;

    bool halted = true;
};

class WarfStation : public Station
{
    using Station::Station;

    void tick(float dt) override;
    void orderShip(ShipConstructionOrder order);

    bool doesStationHaveAOrderInQueue(int stationID);

private:
    std::vector<ShipConstructionOrder> shipConstructors;

    void postUpdateInventory() override;
};