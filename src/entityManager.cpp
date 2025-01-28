// #pragma once

// #include "ship.hpp"

// #include <vector>
// #include <memory>
// #include <algorithm>

// class Station;
// class Ship;

// class EntityManager : public std::enable_shared_from_this<EntityManager>
// {
// public:
//     EntityManager() = default;

//     void addShip(std::shared_ptr<Ship> ship)
//     {
//         ship->setManager(shared_from_this());
//         m_Ships.push_back(ship);
//     }
//     void removeShip(std::shared_ptr<Ship> ship)
//     {
//         m_Ships.erase(std::remove(m_Ships.begin(), m_Ships.end(), ship), m_Ships.end());
//     }
//     void addStation(std::shared_ptr<Station> station)
//     {
//         m_Stations.push_back(station);
//     }
//     void removeStation(std::shared_ptr<Station> station)
//     {
//         m_Stations.erase(std::remove(m_Stations.begin(), m_Stations.end(), station), m_Stations.end());
//     }

//     const std::vector<std::shared_ptr<Ship>> &getShips() const
//     {
//         return m_Ships;
//     }
//     const std::vector<std::shared_ptr<Station>> &getStations() const
//     {
//         return m_Stations;
//     }

// private:
//     std::vector<std::shared_ptr<Ship>> m_Ships;
//     std::vector<std::shared_ptr<Station>> m_Stations;
// };

#include "entityManager.hpp"
#include "ship.hpp"
#include "station.hpp"
#include "warfStation.hpp"

void EntityManager::addShip(std::shared_ptr<Ship> ship)
{
    ship->setManager(shared_from_this());
    m_Ships.push_back(ship);
}

void EntityManager::removeShip(std::shared_ptr<Ship> ship)
{
    m_Ships.erase(std::remove(m_Ships.begin(), m_Ships.end(), ship), m_Ships.end());
}

void EntityManager::addStation(std::shared_ptr<Station> station)
{
    m_Stations.push_back(station);
}

void EntityManager::removeStation(std::shared_ptr<Station> station)
{
    m_Stations.erase(std::remove(m_Stations.begin(), m_Stations.end(), station), m_Stations.end());
}

void EntityManager::addWarfStation(std::shared_ptr<WarfStation> warfStation)
{
    m_WarfStations.push_back(warfStation);
    this->addStation(warfStation);
}

void EntityManager::removeWarfStation(std::shared_ptr<WarfStation> warfStation)
{
    m_WarfStations.erase(std::remove(m_WarfStations.begin(), m_WarfStations.end(), warfStation), m_WarfStations.end());
    this->removeStation(warfStation);
}

std::shared_ptr<Station> EntityManager::getStationById(int id)
{
    for (auto &station : m_Stations)
    {
        if (station->getId() == id)
        {
            return station;
        }
    }

    return nullptr;
}
