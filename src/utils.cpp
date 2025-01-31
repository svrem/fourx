#include "utils.hpp"

#include "entityManager.hpp"
#include "ship.hpp"
#include "station.hpp"
#include "wares.hpp"
#include "warfStation.hpp"

#include <SDL2/SDL.h>

#include <iostream>
#include <algorithm>
#include <random>
#include <map>
#include <memory>

int utils::generateId()
{
    static int generatedId = 0;
    return generatedId++;
}

std::random_device utils::rd;
std::mt19937 utils::gen(utils::rd());

struct MaxSellBuyOffersQuantities
{
    int maxSellQuantity = 0;
    int maxBuyQuantity = 0;

    int totalSellQuantity = 0;
    int totalBuyQuantity = 0;

    std::shared_ptr<Station> seller = nullptr;
    std::shared_ptr<Station> buyer = nullptr;
};

void shipPurchaseCheck(std::shared_ptr<EntityManager> entityManager, Uint64 &lastTradeVolumeCheck)
{
    lastTradeVolumeCheck = SDL_GetPerformanceCounter();

    std::map<Ware, MaxSellBuyOffersQuantities> maxGlobalSellBuyOffers;

    for (auto station : entityManager->getStations())
    {
        auto &sellOffers = station->getSellOffers();

        for (auto &sellOffer : sellOffers)
        {
            if (maxGlobalSellBuyOffers.find(sellOffer.first) == maxGlobalSellBuyOffers.end())
            {
                MaxSellBuyOffersQuantities maxSellBuyOffersQuantities{sellOffer.second.quantity, 0, sellOffer.second.quantity, 0, station, nullptr};
                maxGlobalSellBuyOffers[sellOffer.first] = maxSellBuyOffersQuantities;
                continue;
            }

            maxGlobalSellBuyOffers[sellOffer.first].totalSellQuantity += sellOffer.second.quantity;

            if (sellOffer.second.quantity > maxGlobalSellBuyOffers[sellOffer.first].maxSellQuantity)
            {
                maxGlobalSellBuyOffers[sellOffer.first].maxSellQuantity = sellOffer.second.quantity;
                maxGlobalSellBuyOffers[sellOffer.first].seller = station;
            }
        }

        auto &buyOffers = station->getBuyOffers();

        for (auto buyOffer : buyOffers)
        {
            if (maxGlobalSellBuyOffers.find(buyOffer.first) == maxGlobalSellBuyOffers.end())
            {
                MaxSellBuyOffersQuantities maxSellBuyOffersQuantities{0, buyOffer.second.quantity, 0, buyOffer.second.quantity, nullptr, station};

                maxGlobalSellBuyOffers[buyOffer.first] = maxSellBuyOffersQuantities;
                continue;
            }

            maxGlobalSellBuyOffers[buyOffer.first].totalBuyQuantity += buyOffer.second.quantity;

            if (buyOffer.second.quantity > maxGlobalSellBuyOffers[buyOffer.first].maxBuyQuantity)
            {
                maxGlobalSellBuyOffers[buyOffer.first].maxBuyQuantity = buyOffer.second.quantity;
                maxGlobalSellBuyOffers[buyOffer.first].buyer = station;
            }
        }
    }

    auto biggestTradeVolume = std::max_element(maxGlobalSellBuyOffers.begin(), maxGlobalSellBuyOffers.end(), [](const auto &a, const auto &b)
                                               { return std::min(a.second.totalSellQuantity, a.second.totalBuyQuantity) < std::min(b.second.totalSellQuantity, b.second.totalBuyQuantity); });

    int tradeVolume = std::min(biggestTradeVolume->second.totalSellQuantity, biggestTradeVolume->second.totalBuyQuantity);

    // if there is not enough trade volume, don't do anything
    if (tradeVolume < 500)
        return;

    Ware highestVolumeWare = biggestTradeVolume->first;
    std::shared_ptr<Station> station;

    if (biggestTradeVolume->second.totalSellQuantity > biggestTradeVolume->second.totalBuyQuantity)
    {
        station = biggestTradeVolume->second.buyer;
    }
    else
    {
        station = biggestTradeVolume->second.seller;
    }

    if (!station)
        return;

    // add one ship to the station
    ShipConstructionOrder order;
    order.cargoCapacity = 100;
    order.maxSpeed = 600;
    order.ownerID = station->getId();
    order.timeToConstruct = 10;
    order.weaponAttack = 1.0;

    auto &warfStations = entityManager->getWarfStations();

    // DON'T DO THIS
    if (!warfStations[0]->doesStationHaveAOrderInQueue(station->getId()))
    {
        printf("Ordering ship for station %s\n", station->getName().c_str());
        warfStations[0]->orderShip(order);
    }
}
