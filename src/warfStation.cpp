#include "warfStation.hpp"
#include "wares.hpp"
#include "ship.hpp"
#include "entityManager.hpp"

#include <memory>

void WarfStation::orderShip(ShipConstructionOrder order)
{
    this->shipConstructors.push_back(order);
}

void WarfStation::tick(float dt)
{
    // first 5 ships in the queue are constructed
    for (size_t i = 0; i < 5 && i < this->shipConstructors.size(); i++)
    {

        auto &order = this->shipConstructors[i];

        if (order.halted)
            continue;

        order.timeToConstruct -= dt;

        if (order.timeToConstruct <= 0)
        {
            auto ship = std::make_shared<Ship>(this->getPosition(), order.maxSpeed, order.cargoCapacity, order.weaponAttack, this->m_Renderer);
            ship->claim(m_Manager->getStationById(order.ownerID));

            this->m_Manager->addShip(ship);

            this->shipConstructors.erase(this->shipConstructors.begin() + i);
            i--;
        }
    }
}

void WarfStation::postUpdateInventory()
{
    for (size_t i = 0; i < this->shipConstructors.size(); i++)
    {
        auto &order = this->shipConstructors[i];

        if (!order.halted)
            continue;

        order.halted = false;

        for (auto &inputWare : wares::shipConstructionCost)
        {
            if (this->inventory[inputWare.ware] < inputWare.quantity)
            {
                order.halted = true;
                break;
            }
        }

        if (order.halted)
            continue;

        for (auto &inputWare : wares::shipConstructionCost)
        {
            this->updateInventory(inputWare.ware, -inputWare.quantity);
        }
    }
}

bool WarfStation::doesStationHaveAOrderInQueue(int stationID)
{
    for (auto &order : this->shipConstructors)
    {
        if (order.ownerID == stationID)
        {
            return true;
        }
    }

    return false;
}
