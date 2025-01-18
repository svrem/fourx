#include "ship.hpp"

#include "spdlog/spdlog.h"

#include <algorithm>
#include <iostream>
#include <random>

Ship::Ship(vec2f position, float maxSpeed, float cargoCapacity) : m_position(position), maxSpeed(maxSpeed), cargoCapacity(cargoCapacity)
{
    this->id = utils::generateId();
}

void Ship::claim(std::shared_ptr<Station> station)
{
    this->owner = station;
}

void Ship::dock(std::shared_ptr<Station> station)
{
    this->dockedStation = station;
}

void Ship::searchForTrade(const std::vector<std::shared_ptr<Station>> &stations)
{
    if (this->m_orders.size() > 0)
    {
        return;
    }

    spdlog::debug("Searching for trade for ship {}", this->id);

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

        this->addOrder(orders::Undock{});

        if (type == wares::TradeType::Buy)
        {
            float quantity = std::min(sellOffersOwner[ware].quantity, buyOffersStation[ware].quantity);

            this->owner->acceptTrade(wares::TradeType::Sell, ware, quantity);
            station->acceptTrade(wares::TradeType::Buy, ware, quantity);

            this->addOrder(orders::DockAtStation{
                owner,
            });
            this->addOrder(orders::TradeWithStation{
                owner,
                wares::TradeType::Buy,
                ware,
                quantity,
            });
            this->addOrder(orders::Undock{});
            this->addOrder(orders::DockAtStation{
                station,
            });
            this->addOrder(orders::TradeWithStation{
                station,
                wares::TradeType::Sell,
                ware,
                quantity,
            });
        }
        else if (type == wares::TradeType::Sell)
        {
            float quantity = std::min(buyOffersOwner[ware].quantity, sellOffersStation[ware].quantity);

            this->owner->acceptTrade(wares::TradeType::Buy, ware, quantity);
            station->acceptTrade(wares::TradeType::Sell, ware, quantity);

            this->addOrder(orders::DockAtStation{
                station,
            });
            this->addOrder(orders::TradeWithStation{
                station,
                wares::TradeType::Buy,
                ware,
                quantity,
            });
            this->addOrder(orders::Undock{});
            this->addOrder(orders::DockAtStation{
                this->owner,
            });
            this->addOrder(orders::TradeWithStation{
                this->owner,
                wares::TradeType::Sell,
                ware,
                quantity,
            });
        }

        this->executeNextOrder();
        break;
    }

    // this->undock();
    // this->setTarget(station);
}

void Ship::addOrder(ShipOrder order)
{
    this->m_orders.push_back(order);
}

void Ship::executeNextOrder()
{
    if (this->m_orders.size() == 0)
    {
        return;
    }

    spdlog::debug("Executing next order for ship {}", this->id);

    ShipOrder order = this->m_orders[0];
    this->m_orders.erase(this->m_orders.begin());

    if (std::holds_alternative<orders::DockAtStation>(order))
    {
        spdlog::debug("ShipOrder: Docking at station");
        auto dockOrder = std::get<orders::DockAtStation>(order);
        this->setTarget(dockOrder.station);
    }
    else if (std::holds_alternative<orders::TradeWithStation>(order))
    {
        spdlog::debug("ShipOrder: Trading with station");
        auto tradeOrder = std::get<orders::TradeWithStation>(order);
        assert(this->dockedStation == tradeOrder.station);

        if (tradeOrder.type == wares::TradeType::Buy)
        {
            this->dockedStation->transferWares(this->shared_from_this(), tradeOrder.ware, tradeOrder.quantity);
        }
        else if (tradeOrder.type == wares::TradeType::Sell)
        {
            this->dockedStation->transferWares(this->shared_from_this(), tradeOrder.ware, -tradeOrder.quantity);
        }

        this->executeNextOrder();

        // // transfer wares
        // if (tradeOrder.type == wares::TradeType::Buy)
        // {
        //     this->cargo[tradeOrder.ware] += tradeOrder.quantity;
        //     this->dockedStation->updateInventory(tradeOrder.ware, -tradeOrder.quantity);
        // }
        // else if (tradeOrder.type == wares::TradeType::Sell)
        // {
        //     this->cargo[tradeOrder.ware] -= tradeOrder.quantity;
        // }
    }
    else if (std::holds_alternative<orders::Undock>(order))
    {
        spdlog::debug("ShipOrder: Undocking");
        this->undock();
        this->executeNextOrder();
    }
}

void Ship::addWare(Ware ware, int quantity)
{
    if (this->cargo.find(ware) == this->cargo.end())
    {
        this->cargo[ware] = 0;
    }

    this->cargo[ware] += quantity;
}

void Ship::undock()
{
    this->dockedStation = nullptr;
}

void Ship::setTarget(vec2f target)
{
    this->m_target = target;
}

void Ship::setTarget(std::shared_ptr<Station> station)
{
    const static float offset = 1;

    const vec2f &stationPosition = station->getPosition();

    float deltaX = stationPosition.x - this->m_position.x;
    float deltaY = stationPosition.y - this->m_position.y;
    float alpha = atan2(deltaY, deltaX);

    float x = stationPosition.x - offset * cos(alpha);
    float y = stationPosition.y - offset * sin(alpha);

    this->targetStation = station;
    this->setTarget(vec2f(x, y));
}

void Ship::tick(float dt)
{
    if (this->dockedStation != nullptr)
    {
        return;
    }

    if (this->m_target.has_value() == false)
    {
        return;
    }

    vec2f target = this->m_target.value();

    float deltaX = target.x - this->m_position.x;
    float deltaY = target.y - this->m_position.y;
    float distance2 = deltaX * deltaX + deltaY * deltaY;

    if (distance2 < this->maxSpeed * dt * this->maxSpeed * dt)
    {
        this->m_position = target;

        if (this->targetStation != nullptr)
        {
            this->targetStation->requestDock(this->shared_from_this());
            this->targetStation = nullptr;
        }

        this->m_target.reset();
        this->executeNextOrder();

        return;
    }

    float alpha = atan2(deltaY, deltaX);

    float x = this->m_position.x + this->maxSpeed * dt * cos(alpha);
    float y = this->m_position.y + this->maxSpeed * dt * sin(alpha);

    this->m_position = vec2f(x, y);
}

void Ship::render(SDL_Renderer *renderer)
{
    if (this->dockedStation != nullptr)
    {
        return;
    }

    SDL_Rect dest;
    dest.x = this->m_position.x;
    dest.y = this->m_position.y;
    dest.w = 10;
    dest.h = 10;

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer, &dest);
}