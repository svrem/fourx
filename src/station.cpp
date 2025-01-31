#include "station.hpp"
#include "config.hpp"
#include "ui.hpp"

#include "SDL2/SDL_image.h"
#include "SDL2/SDL_ttf.h"

#include <iostream>
#include <cassert>
#include <set>

Station::Station(vec2f position, std::string_view name, std::shared_ptr<EntityManager> entityManager, std::shared_ptr<UI> ui, SDL_Renderer *renderer, TTF_Font *font) : m_Position(position), name(name), m_Renderer(renderer), m_Manager(entityManager), m_UI(ui)
{
    id = utils::generateId();

    m_Texture = IMG_LoadTexture(renderer, "assets/station.png");

    if (!font)
    {
        throw std::runtime_error("Failed to load font");
    }

    SDL_Surface *nameSurface = TTF_RenderText_Blended(font, name.data(), {255, 255, 255});
    m_NameTexture = SDL_CreateTextureFromSurface(renderer, nameSurface);
    m_NameTextWidth = nameSurface->w;
    m_NameTextHeight = nameSurface->h;
    SDL_FreeSurface(nameSurface);
}

Station::~Station()
{
    SDL_DestroyTexture(m_Texture);
    SDL_DestroyTexture(m_NameTexture);
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

    this->postUpdateInventory();
    this->reevaluateTradeOffers();

    this->updateUI();
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

// Reevaluates the trade offers for the station based on the current inventory levels
// and the reservations made by ships.
void Station::reevaluateTradeOffers()
{
    for (auto const &[ware, inventoryLevel] : inventory)
    {
        if (maintenanceLevels.find(ware) == maintenanceLevels.end())
        {
            // warning
            // std::cerr << "Warning: Maintenance level for ware " << wares::wareDetails.at(ware).name << " has not been set.\n";
            continue;
        }
        int level = inventoryLevel + buyReservations[ware];
        int maintenanceLevelDiff = level - maintenanceLevels.at(ware);
        wares::TradeType type = maintenanceLevelDiff >= 0 ? wares::TradeType::Sell : wares::TradeType::Buy;
        int quantity = maintenanceLevelDiff > 0 ? maintenanceLevelDiff : -maintenanceLevelDiff;
        // if (quantity == 0)
        // continue;
        // price change HIER!!! maintenance levels kunnen ook 0 zijn slimpie, dus division by zero
        // float b = maintenanceLevelDiff > 0 ? maintenanceLevelDiff : 1000 - maintenanceLevelDiff;
        // float a = MAX_ALLOWED_PRICE_CHANGE_PERCENTAGE / pow(maintenanceLevelDiff, PRICE_CHANGE_EXPONENT);
        // float a = MAX_ALLOWED_PRICE_CHANGE_PERCENTAGE / pow(1000, PRICE_CHANGE_EXPONENT);
        // float priceChangePercentage = a * pow(maintenanceLevelDiff, PRICE_CHANGE_EXPONENT);
        float a = MAX_ALLOWED_PRICE_CHANGE_PERCENTAGE / pow(MAX_EXPECTED_PRODUCT_COUNT, PRICE_CHANGE_EXPONENT);
        float priceChangePercentage = a * pow(-maintenanceLevelDiff, PRICE_CHANGE_EXPONENT);
        priceChangePercentage = std::min(priceChangePercentage, static_cast<float>(MAX_ALLOWED_PRICE_CHANGE_PERCENTAGE));
        updateTradeOffer(type, ware, quantity, priceChangePercentage);
    };
}

void Station::setMaintenanceLevel(Ware ware, int level)
{
    if (inventory.find(ware) == inventory.end())
    {
        inventory[ware] = 0;
    }
    maintenanceLevels[ware] = level;
}
// Accepts a trade offer for a specific ware, in this case, the TradeType should be of the offer
// that's being accepted (i.e. if the client is buying, the TradeType should be Sell, and vice versa)
// Throws an exception if the trade is invalid (e.g. not enough inventory to sell).
void Station::acceptTrade(wares::TradeType type, Ware ware, int quantity)
{
    if (type == wares::TradeType::Sell)
    {
        if (inventory[ware] < quantity)
            throw std::runtime_error("Not enough inventory to sell");
        if (sellReservations.find(ware) == sellReservations.end())
        {
            sellReservations[ware] = 0;
        }
        sellReservations[ware] += quantity;
        updateInventory(ware, -quantity);
    }
    else
    {
        if (buyReservations.find(ware) == buyReservations.end())
        {
            buyReservations[ware] = 0;
        }
        buyReservations[ware] += quantity;
    }
    reevaluateTradeOffers();
}

void Station::updateUI()
{
    if (!this->m_Selected)
        return;

    UISupport::DataDisplay dataDisplay;

    dataDisplay.push_back({"Station", name});
    dataDisplay.push_back({"Position", std::to_string((int)m_Position.x) + ", " + std::to_string((int)m_Position.y)});

    for (auto &item : inventory)
    {
        auto details = wares::wareDetails.at(item.first);
        if (details.name.empty())
            continue;

        dataDisplay.push_back({details.name, std::to_string(item.second)});
    }

    for (auto &item : sellOffers)
    {
        auto details = wares::wareDetails.at(item.first);
        if (details.name.empty())
            continue;

        dataDisplay.push_back({details.name + " sell price", std::to_string(item.second.price)});
        dataDisplay.push_back({details.name + " sell quantity", std::to_string(item.second.quantity)});
    }

    for (auto &item : buyOffers)
    {
        auto details = wares::wareDetails.at(item.first);
        if (details.name.empty())
            continue;

        dataDisplay.push_back({details.name + " buy price", std::to_string(item.second.price)});
        dataDisplay.push_back({details.name + " buy quantity", std::to_string(item.second.quantity)});
    }

    m_UI->setUIData({name, dataDisplay});
}

bool Station::checkForAndHandleMouseClick(vec2f camera, Sint32 x, Sint32 y)
{
    int x1 = x + camera.x;
    int y1 = y + camera.y;

    if (x1 >= m_Position.x - 15 && x1 <= m_Position.x + 15 && y1 >= m_Position.y - 15 && y1 <= m_Position.y + 15)
    {
        this->m_Selected = !this->m_Selected;
        this->updateUI();

        return true;
    }

    this->m_Selected = false;
    return false;
}

void Station::deselect()
{
    this->m_Selected = false;
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

    SDL_RenderCopy(m_Renderer, m_Texture, NULL, &dest);

    SDL_Rect nameDest;
    nameDest.x = position.x - m_NameTextHeight / 2;
    nameDest.y = position.y + 30;
    nameDest.w = m_NameTextWidth;
    nameDest.h = m_NameTextHeight;

    SDL_RenderCopy(m_Renderer, m_NameTexture, NULL, &nameDest);

    if (m_Selected)
    {
        SDL_SetRenderDrawColor(m_Renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
    }
    else
    {
        SDL_SetRenderDrawColor(m_Renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    }

    SDL_RenderDrawRect(m_Renderer, &dest);
}