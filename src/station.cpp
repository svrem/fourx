#include "station.hpp"
#include "config.hpp"

#include "SDL2/SDL_image.h"
#include "SDL2/SDL_ttf.h"
#include "spdlog/spdlog.h"

#include <iostream>
#include <cassert>
#include <set>

Station::Station(vec2f position, std::string_view name, std::shared_ptr<EntityManager> entityManager, SDL_Renderer *renderer, TTF_Font *font) : m_Position(position), name(name), renderer(renderer), entityManager(entityManager)
{
    id = utils::generateId();

    texture = IMG_LoadTexture(renderer, "assets/station.png");

    SDL_Surface *nameSurface = TTF_RenderText_Blended(font, name.data(), {255, 255, 255});
    nameTexture = SDL_CreateTextureFromSurface(renderer, nameSurface);
    nameTextWidth = nameSurface->w;
    nameTextHeight = nameSurface->h;
    SDL_FreeSurface(nameSurface);
}

void Station::addShip(std::shared_ptr<Ship> ship)
{
    ship->claim(shared_from_this());
    owned_ships.push_back(std::move(ship));
    printf("Ship added to station %d with ID=%d, current fleet size %d.\n", id, owned_ships.back()->getId(), owned_ships.size());
}

void Station::removeShip(int ship_id)
{
    for (int i = 0; i < owned_ships.size(); i++)
    {

        if (owned_ships[i]->getId() == ship_id)
        {
            owned_ships.erase(owned_ships.begin() + i);
            return;
        }
    }

    throw std::runtime_error("Ship not found");
}

void Station::updateInventory(Ware ware, int quantity)
{
    if (inventory.find(ware) == inventory.end())
    {
        inventory[ware] = 0;
    }
    inventory[ware] += quantity;

    assert(inventory[ware] >= 0);

    for (auto &productionModule : this->productionModules)
    {
        // if production module is halted, check if we can start a new cycle
        if (productionModule.halted)
        {
            printf("Production module is halted\n");
            startNewProductionCycle(productionModule);
        }
    }

    this->reevaluateTradeOffers();
}

void Station::__debug_print_inventory() const
{
    std::cout << "===========================================\n";
    std::cout << "Inventory for station " << id << "\n\n";
    for (auto &item : inventory)
    {
        auto details = wares::wareDetails.at(item.first);
        std::cout << "Ware: " << details.name << "; Quantity: " << item.second << "\n";
    }
    std::cout << "\n===========================================" << std::endl;
}

void Station::startNewProductionCycle(ProductionModule &productionModule)
{
    productionModule.halted = false;
    for (auto &inputWare : productionModule.inputWares)
    {
        if (inventory[inputWare.ware] < inputWare.quantity)
        {
            productionModule.halted = true;
            break;
        }
    }

    if (productionModule.halted)
    {
        // not enough input wares
        productionModule.current_cycle_time = 0;
        return;
    }

    for (auto &inputWare : productionModule.inputWares)
    {
        printf("Remainder of ware %d: %d\n", inputWare.ware, inventory[inputWare.ware] - inputWare.quantity);
        this->updateInventory(inputWare.ware, -inputWare.quantity);
    }
}

void Station::tick(float dt)
{
    for (auto &productionModule : this->productionModules)
    {
        if (productionModule.halted)
            continue;

        productionModule.current_cycle_time += dt;

        if (productionModule.current_cycle_time < productionModule.cycle_time)
            continue;

        for (auto &outputWare : productionModule.outputWares)
        {
            if (std::holds_alternative<wares::WareQuantity>(outputWare))
            {
                auto wareQuantity = std::get<wares::WareQuantity>(outputWare);
                this->updateInventory(wareQuantity.ware, wareQuantity.quantity);
                continue;
            }
            else if (std::holds_alternative<wares::ShipOrder>(outputWare))
            {
                auto shipOrder = std::get<wares::ShipOrder>(outputWare);
                auto ship = std::make_shared<Ship>(this->m_Position, shipOrder.maxSpeed, shipOrder.cargoCapacity, shipOrder.weaponAttack, renderer);

                continue;
            }
        }

        productionModule.current_cycle_time -= productionModule.cycle_time;

        spdlog::info("Station {} produced wares in production module", id);

        // start new cycle
        startNewProductionCycle(productionModule);
    }
}

void Station::updateTradeOffer(wares::TradeType type, Ware ware, int quantity, float priceChangePercentage)
{
    bool hasSellOffer = this->sellOffers.find(ware) != this->sellOffers.end();
    bool hasBuyOffer = this->buyOffers.find(ware) != this->buyOffers.end();

    if (quantity == 0)
    {
        if (hasSellOffer)
        {
            sellOffers[ware] = {sellOffers[ware].price, 0};
        }
        else if (hasBuyOffer)
        {
            buyOffers[ware] = {buyOffers[ware].price, 0};
        }

        return;
    }

    float max_min_ware_price = wares::wareDetails.at(ware).max_price - wares::wareDetails.at(ware).min_price;

    if (type == wares::TradeType::Sell)
    {
        float price;

        if (hasSellOffer)
        {
            price = this->sellOffers[ware].price + max_min_ware_price * (priceChangePercentage + 0.00001);
        }
        else
        {
            price = wares::wareDetails.at(ware).max_price;
        }

        price = std::min(price, wares::wareDetails.at(ware).max_price);
        price = std::max(price, wares::wareDetails.at(ware).min_price);

        sellOffers[ware] = {price, quantity};
        buyOffers.erase(ware);

        return;
    }

    float price;

    if (hasBuyOffer)
    {
        price = this->buyOffers[ware].price + max_min_ware_price * (priceChangePercentage - 0.00001);
    }
    else
    {
        price = wares::wareDetails.at(ware).min_price;
    }

    price = std::min(price, wares::wareDetails.at(ware).max_price);
    price = std::max(price, wares::wareDetails.at(ware).min_price);

    buyOffers[ware] = {price, quantity};
    sellOffers.erase(ware);
}

// Transfers wares between the station and a ship. The quantity should be positive if the ship is buying,
// and negative if the ship is selling. Throws an exception if the trade is invalid (e.g. not enough inventory to sell).
void Station::transferWares(std::shared_ptr<Ship> ship, Ware ware, int quantity)
{
    if (sellReservations[ware] < quantity)
    {
        throw std::runtime_error("Not enough inventory to transfer");
    }

    if (ship->getCargoSpace() < quantity)
    {
        throw std::runtime_error("Not enough cargo space to transfer");
    }

    if (quantity < 0)
    {
        buyReservations[ware] -= -quantity;
        this->updateInventory(ware, -quantity);
    }
    else
    {
        sellReservations[ware] += quantity;
    }

    ship->addWare(ware, quantity);
    this->reevaluateTradeOffers();
}

void Station::requestDock(std::shared_ptr<Ship> ship)
{
    if (docked_ships.size() < m_max_docked_ships)
    {
        docked_ships.push_back(ship);
        ship->dock(shared_from_this());
        return;
    }

    dock_queue.push_back(ship);
}

void Station::undock(std::shared_ptr<Ship> ship)
{

    for (int i = 0; i < docked_ships.size(); i++)
    {
        if (docked_ships[i]->getId() == ship->getId())
        {
            docked_ships.erase(docked_ships.begin() + i);
            return;
        }
    }

    throw std::runtime_error("Ship not found");
}

// SDL
void Station::render(vec2f camera)
{
    vec2f position = m_Position - camera;

    SDL_Rect dest;
    dest.x = position.x - 15;
    dest.y = position.y - 15;
    dest.w = 30;
    dest.h = 30;

    SDL_RenderCopy(renderer, texture, NULL, &dest);

    SDL_Rect nameDest;
    nameDest.x = position.x - nameTextWidth / 2;
    nameDest.y = position.y + 30;
    nameDest.w = nameTextWidth;
    nameDest.h = nameTextHeight;

    SDL_RenderCopy(renderer, nameTexture, NULL, &nameDest);

    // DEBUG
    if (buyOffers.size() > 0)
    {

        SDL_Rect priceRect;
        priceRect.x = position.x - 15;
        priceRect.y = position.y - 30;
        priceRect.w = this->buyOffers[Ware::Silicon].price / 5.0 * 100.0;
        priceRect.h = 5;

        SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderFillRect(renderer, &priceRect);
    }

    if (sellOffers.size() > 0)
    {

        SDL_Rect priceRect;
        priceRect.x = position.x - 15;
        priceRect.y = position.y - 35;
        priceRect.w = this->sellOffers[Ware::Silicon].price / 5.0 * 100.0;
        priceRect.h = 5;

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderFillRect(renderer, &priceRect);
    }
}