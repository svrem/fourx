#include "ship.hpp"

#include <algorithm>
#include <iostream>

Ship::Ship(vec2f position, float maxSpeed, float cargoCapacity) : position(position), maxSpeed(maxSpeed), cargoCapacity(cargoCapacity)
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

void Ship::searchForTrade(const std::vector<std::shared_ptr<Station>> &stations)
{
    std::vector<size_t> station_indices;
    station_indices.reserve(stations.size());

    // populate station_indices
    for (size_t i = 0; i < stations.size(); i++)
    {
        if (stations[i]->getId() == this->owner->getId())
        {
            continue;
        }

        station_indices.push_back(i);
    }

    std::shuffle(station_indices.begin(), station_indices.end(), utils::gen);

    auto buyOffersOwner = this->owner->getBuyOffers();
    auto sellOffersOwner = this->owner->getSellOffers();

    for (size_t index : station_indices)
    {

        auto station = stations[index];

        auto buyOffersStation = station->getBuyOffers();
        auto sellOffersStation = station->getSellOffers();

        std::vector<std::pair<wares::TradeType, wares::Ware>> possibleTrades;

        for (auto const &[wareOwner, offerOwner] : buyOffersOwner)
        {
            if (sellOffersStation.find(wareOwner) == sellOffersStation.end())
                continue;

            if (offerOwner.quantity == 0 || sellOffersStation[wareOwner].quantity == 0)
                continue;

            if (sellOffersStation[wareOwner].price > offerOwner.price)
                continue;

            possibleTrades.push_back({wares::TradeType::Sell, wareOwner});
        }

        for (auto const &[wareOwner, offerOwner] : sellOffersOwner)
        {
            if (buyOffersStation.find(wareOwner) == buyOffersStation.end())
                continue;

            if (offerOwner.quantity == 0 || buyOffersStation[wareOwner].quantity == 0)
                continue;

            if (buyOffersStation[wareOwner].price < offerOwner.price)
                continue;

            possibleTrades.push_back({wares::TradeType::Buy, wareOwner});
        }

        if (possibleTrades.size() == 0)
            continue;

        auto &trade = possibleTrades[utils::gen() % possibleTrades.size()];
        wares::TradeType type = trade.first;
        wares::Ware ware = trade.second;

        if (type == wares::TradeType::Buy)
        {
            float quantity = std::min(sellOffersOwner[ware].quantity, buyOffersStation[ware].quantity);

            this->owner->acceptTrade(wares::TradeType::Sell, ware, quantity);
            station->acceptTrade(wares::TradeType::Buy, ware, quantity);
        }
        else
        {
            float quantity = std::min(buyOffersOwner[ware].quantity, sellOffersStation[ware].quantity);

            this->owner->acceptTrade(wares::TradeType::Buy, ware, quantity);
            station->acceptTrade(wares::TradeType::Sell, ware, quantity);
        }
    }
}