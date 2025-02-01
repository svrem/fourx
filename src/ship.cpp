#include "ship.hpp"
#include "station.hpp"
#include "vec.hpp"
#include "utils.hpp"
#include "wares.hpp"
#include "orders.hpp"
#include "entityManager.hpp"

#include <algorithm>
#include <iostream>
#include <random>
#include <cassert>

Ship::Ship(vec2f m_Position, float maxSpeed, float cargoCapacity, float weaponAttack, SDL_Renderer *renderer) : m_Position(m_Position), maxSpeed(maxSpeed), cargoCapacity(cargoCapacity), weaponAttack(weaponAttack), m_Renderer(renderer)
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
    this->executeNextOrder();
}

void Ship::searchForTrade(const std::vector<std::shared_ptr<Station>> &stations, float dt)
{
    if (this->m_Orders.size() > 0)
    {
        return;
    }

    if (this->owner == nullptr)
    {
        return;
    }

    if (this->m_TimeUntilNextTradeCheck > 0)
    {
        this->m_TimeUntilNextTradeCheck -= dt;
        return;
    }

    this->m_TimeUntilNextTradeCheck = static_cast<float>(utils::gen() % 60);

    std::vector<size_t> station_indices;
    station_indices.reserve(stations.size());

    // populate station_indices
    for (size_t i = 0; i < stations.size(); i++)
    {
        if (stations[i]->getId() == this->owner->getId())
        {
            continue;
        }

        // float distance = vec2f::distance(this->m_Position, stations[i]->getPosition());
        auto &targetStationPosition = stations[i]->getPosition();
        float deltaX = targetStationPosition.x - this->m_Position.x;
        float deltaY = targetStationPosition.y - this->m_Position.y;

        float distance = deltaX * deltaX + deltaY * deltaY;

        static const float maxDistance = 1000 * 1000; // squared distance
        // if (distance > maxDistance)
        // {
        //     continue;
        // }

        station_indices.push_back(i);
    }

    // std::shuffle(station_indices.begin(), station_indices.end(), utils::gen);
    std::sort(station_indices.begin(), station_indices.end(), [&](size_t a, size_t b)
              {
        auto &targetStationPositionA = stations[a]->getPosition();
        auto &targetStationPositionB = stations[b]->getPosition();

        float deltaXA = targetStationPositionA.x - this->m_Position.x;
        float deltaYA = targetStationPositionA.y - this->m_Position.y;
        float distanceA = deltaXA * deltaXA + deltaYA * deltaYA;

        float deltaXB = targetStationPositionB.x - this->m_Position.x;
        float deltaYB = targetStationPositionB.y - this->m_Position.y;
        float distanceB = deltaXB * deltaXB + deltaYB * deltaYB;

        return distanceA < distanceB; });

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
            int quantity = std::min(sellOffersOwner[ware].quantity, buyOffersStation[ware].quantity);
            quantity = std::min(quantity, this->cargoCapacity - this->m_Cargo[ware]);

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
            int quantity = std::min(buyOffersOwner[ware].quantity, sellOffersStation[ware].quantity);
            quantity = std::min(quantity, this->cargoCapacity - this->m_Cargo[ware]);

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

    ShipOrder order = this->m_Orders[0];
    this->m_Orders.erase(this->m_Orders.begin());

    if (std::holds_alternative<orders::DockAtStation>(order))
    {
        auto dockOrder = std::get<orders::DockAtStation>(order);

        assert(this->dockedStation == nullptr);

        this->setTarget(dockOrder.station);
    }
    else if (std::holds_alternative<orders::TradeWithStation>(order))
    {
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
        this->undock();
    }
    else if (std::holds_alternative<orders::MoveToPosition>(order))
    {
        auto moveOrder = std::get<orders::MoveToPosition>(order);

        this->setTarget(moveOrder.position);
    }
    else
    {
        throw std::runtime_error("Unknown order type");
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

        this->m_Position = target;

        this->m_Target.reset();

        if (this->targetStation != nullptr)
        {
            auto t = this->targetStation;
            this->targetStation = nullptr;
            t->requestDock(this->shared_from_this());
        }

        return;
    }

    float alpha = atan2(deltaY, deltaX);
    this->m_CurrentDirection = alpha;

    float x = this->m_Position.x + this->maxSpeed * dt * cos(alpha);
    float y = this->m_Position.y + this->maxSpeed * dt * sin(alpha);

    this->m_Position = vec2f(x, y);
}

void Ship::attack(std::shared_ptr<Ship> target)
{
    while (target->getHullHealth() > 0 && this->getHullHealth() > 0)
    {
        if (utils::gen() % 2 == 0)
        {
            printf("Ship %d attacking ship %d\n", this->id, target->id);
            target->doDamage(this->weaponAttack);
        }
        else
        {
            printf("Ship %d attacking ship %d\n", target->id, this->id);
            this->doDamage(target->weaponAttack);
        }
    }
}

void Ship::setManager(std::shared_ptr<EntityManager> manager)
{
    this->m_Manager = manager;
}

void Ship::doDamage(float damage)
{
    this->hullHealth -= damage;

    if (this->hullHealth <= 0)
    {
        this->m_Manager->removeShip(this->shared_from_this());
    }
}

void Ship::intercept(std::shared_ptr<Ship> target, float dt)
{
    vec2f targetPos = target->m_Position;

    float partialFutureTargetPosX = target->maxSpeed * cos(target->m_CurrentDirection);
    float partialFutureTargetPosY = target->maxSpeed * sin(target->m_CurrentDirection);

    float futureTargetPosX = targetPos.x + partialFutureTargetPosX * dt;
    float futureTargetPosY = targetPos.y + partialFutureTargetPosY * dt;

    float deltaX = futureTargetPosX - this->m_Position.x;
    float deltaY = futureTargetPosY - this->m_Position.y;

    float distance = sqrt(deltaX * deltaX + deltaY * deltaY) - 5;

    if (distance < this->maxSpeed * dt)
    {
        this->attack(target);
        return;
    }

    float sampleT = 0.01;
    float t = 0.0;

    while (true)
    {
        float futureTargetPosX = targetPos.x + partialFutureTargetPosX * t;
        float futureTargetPosY = targetPos.y + partialFutureTargetPosY * t;

        float deltaX = futureTargetPosX - this->m_Position.x;
        float deltaY = futureTargetPosY - this->m_Position.y;

        float distance = sqrt(deltaX * deltaX + deltaY * deltaY);
        float timeToIntercept = distance / this->maxSpeed;

        if (timeToIntercept <= t)
        {
            this->setTarget(vec2f(futureTargetPosX, futureTargetPosY));
            break;
        }
        else
        {
            t += timeToIntercept * 0.2;
        }
    }
}

void Ship::render(vec2f camera, float zoomLevel, vec2f zoomCenter)
{
    if (this->dockedStation != nullptr)
    {
        return;
    }

    vec2f position = this->m_Position - camera;

    SDL_Rect dest;
    dest.x = (position.x - zoomCenter.x) * zoomLevel + zoomCenter.x - 5 * zoomLevel;
    dest.y = (position.y - zoomCenter.y) * zoomLevel + zoomCenter.y - 5 * zoomLevel;
    dest.w = 10 * zoomLevel;
    dest.h = 10 * zoomLevel;

    SDL_SetRenderDrawColor(m_Renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(m_Renderer, &dest);

    if (zoomLevel < 0.5f)
        return;

    static const int maxHealth = 100;
    static const int maxHealthBarWidth = 20;
    SDL_Rect healthBar;
    healthBar.w = this->hullHealth / 100 * maxHealthBarWidth * zoomLevel;
    healthBar.h = 5 * zoomLevel;
    healthBar.x = dest.x - (maxHealthBarWidth / 4) * zoomLevel;
    healthBar.y = dest.y + 15 * zoomLevel;

    SDL_SetRenderDrawColor(m_Renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(m_Renderer, &healthBar);
}