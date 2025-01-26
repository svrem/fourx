#pragma once

#include <vector>
#include <memory>
#include <algorithm>

class Station;
class Ship;

class EntityManager : public std::enable_shared_from_this<EntityManager>
{
public:
    EntityManager() = default;

    void addShip(std::shared_ptr<Ship> ship);
    void removeShip(std::shared_ptr<Ship> ship);

    void addStation(std::shared_ptr<Station> station);
    void removeStation(std::shared_ptr<Station> station);
    std::shared_ptr<Station> getStationById(int id);

    const std::vector<std::shared_ptr<Ship>> &getShips() const
    {
        return m_Ships;
    }
    const std::vector<std::shared_ptr<Station>> &getStations() const
    {
        return m_Stations;
    }

private:
    std::vector<std::shared_ptr<Ship>> m_Ships;
    std::vector<std::shared_ptr<Station>> m_Stations;
};