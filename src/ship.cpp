#include "ship.hpp"

#include "spdlog/spdlog.h"

#include <algorithm>
#include <iostream>
#include <random>

Ship::Ship(vec2f m_Position, float maxSpeed, float cargoCapacity, SDL_Renderer *renderer) : m_Position(m_Position), maxSpeed(maxSpeed), cargoCapacity(cargoCapacity), m_Renderer(renderer)
{
    this->id = utils::generateId();
}

void Ship::claim(std::shared_ptr<Station> station)
{
    this->owner = station;
}

void Ship::dock(std::shared_ptr<Station> station)
{
    spdlog::debug("Ship {} docking at station {}", this->id, station->getId());
    this->dockedStation = station;
    this->executeNextOrder();
}

void Ship::searchForTrade(const std::vector<std::shared_ptr<Station>> &stations)
{
    if (this->m_Orders.size() > 0)
    {
        return;
    }

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
    this->m_Orders.push_back(order);
}

void Ship::executeNextOrder()
{
    if (this->m_Orders.size() == 0)
    {
        return;
    }

    spdlog::debug("Executing next order for ship {}", this->id);

    ShipOrder order = this->m_Orders[0];
    this->m_Orders.erase(this->m_Orders.begin());

    if (std::holds_alternative<orders::DockAtStation>(order))
    {
        spdlog::debug("ShipOrder: Docking at station");
        auto dockOrder = std::get<orders::DockAtStation>(order);

        assert(this->dockedStation == nullptr);

        this->setTarget(dockOrder.station);
    }
    else if (std::holds_alternative<orders::TradeWithStation>(order))
    {
        spdlog::debug("ShipOrder: Trading with station");
        auto tradeOrder = std::get<orders::TradeWithStation>(order);
        assert(this->dockedStation == tradeOrder.station);

        if (tradeOrder.type == wares::TradeType::Buy)
        {
            spdlog::info("Ship {} buying {} units of ware", this->id, tradeOrder.quantity);
            this->dockedStation->transferWares(this->shared_from_this(), tradeOrder.ware, tradeOrder.quantity);
        }
        else if (tradeOrder.type == wares::TradeType::Sell)
        {
            spdlog::info("Ship {} selling {} units of ware", this->id, tradeOrder.quantity);
            this->dockedStation->transferWares(this->shared_from_this(), tradeOrder.ware, -tradeOrder.quantity);
        }

        this->executeNextOrder();

        // // transfer wares
        // if (tradeOrder.type == wares::TradeType::Buy)
        // {
        //     this->m_Cargo[tradeOrder.ware] += tradeOrder.quantity;
        //     this->dockedStation->updateInventory(tradeOrder.ware, -tradeOrder.quantity);
        // }
        // else if (tradeOrder.type == wares::TradeType::Sell)
        // {
        //     this->m_Cargo[tradeOrder.ware] -= tradeOrder.quantity;
        // }
    }
    else if (std::holds_alternative<orders::Undock>(order))
    {
        spdlog::debug("ShipOrder: Undocking");
        this->undock();
    }
}

void Ship::addWare(Ware ware, int quantity)
{
    if (this->m_Cargo.find(ware) == this->m_Cargo.end())
    {
        this->m_Cargo[ware] = 0;
    }

    this->m_Cargo[ware] += quantity;
}

void Ship::undock()
{
    if (this->dockedStation == nullptr)
    {
        executeNextOrder();
        return;
    }

    this->dockedStation->undock(this->shared_from_this());
    this->dockedStation = nullptr;
    this->executeNextOrder();
}

void Ship::setTarget(vec2f target)
{
    this->m_Target = target;

    assert(this->m_Target.has_value());
}

void Ship::setTarget(std::shared_ptr<Station> station)
{
    assert(station != nullptr);

    const static float offset = 0;

    const vec2f &stationPosition = station->getPosition();

    float deltaX = stationPosition.x - this->m_Position.x;
    float deltaY = stationPosition.y - this->m_Position.y;
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

    if (!this->m_Target.has_value())
    {
        return;
    }

    vec2f target = this->m_Target.value();

    float deltaX = target.x - this->m_Position.x;
    float deltaY = target.y - this->m_Position.y;
    float distance2 = deltaX * deltaX + deltaY * deltaY;

    if (distance2 < this->maxSpeed * dt * this->maxSpeed * dt)
    {

        spdlog::debug("Ship {} reached target", this->id);
        this->m_Position = target;

        this->m_Target.reset();

        printf("Requesting dock at station\n");
        if (this->targetStation != nullptr)
        {
            auto t = this->targetStation;
            this->targetStation = nullptr;
            t->requestDock(this->shared_from_this());
        }

        return;
    }

    float alpha = atan2(deltaY, deltaX);

    float x = this->m_Position.x + this->maxSpeed * dt * cos(alpha);
    float y = this->m_Position.y + this->maxSpeed * dt * sin(alpha);

    this->m_Position = vec2f(x, y);
}

void Ship::render(vec2f camera)
{
    if (this->dockedStation != nullptr)
    {
        return;
    }

    vec2f position = this->m_Position - camera;

    SDL_Rect dest;
    dest.x = position.x - 5;
    dest.y = position.y - 5;
    dest.w = 10;
    dest.h = 10;

    SDL_SetRenderDrawColor(m_Renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(m_Renderer, &dest);
}