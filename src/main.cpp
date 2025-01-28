

#include "ship.hpp"
#include "vec.hpp"
#include "entityManager.hpp"
#include "productionStation.hpp"
#include "station.hpp"

#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "spdlog/spdlog.h"
#include "spdlog/cfg/env.h"
#include "spdlog/fmt/ostr.h"

#include <iostream>
#include <vector>
#include <memory>
#include <boost/concept_check.hpp>

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

#define PLAYER_SPEED 500

// You shouldn't really use this statement, but it's fine for small programs
using namespace std;

const SDL_Color SHIP_COLOR = {61, 120, 255, 255};

// You must include the command line parameters for your main function to be recognized by SDL
int main(int argc, char **args)
{
    // Initialize spdlog
    spdlog::cfg::load_env_levels();
    spdlog::info("Initializing ship and station states");

    // Pointers to our window and surface
    SDL_Window *window = NULL;

    if (TTF_Init() < 0)
    {
        cout << "Error initializing TTF: " << SDL_GetError() << endl;
        return 1;
    }

    // Initialize SDL. SDL_Init will return -1 if it fails.
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        cout << "Error initializing SDL: " << SDL_GetError() << endl;
        // End the program
        return 1;
    }

    if (SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1") == SDL_FALSE)
    {
        cout << "Warning: Linear texture filtering not enabled!" << endl;
        return 1;
    }

    // Create our window
    window = SDL_CreateWindow("Example", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    // Make sure creating the window succeeded
    if (!window)
    {
        cout << "Error creating window: " << SDL_GetError() << endl;
        // End the program
        return 1;
    }

    // Get renderer
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (!renderer)
    {
        cout << "Error creating renderer: " << SDL_GetError() << endl;
        // End the program
        return 1;
    }

    IMG_Init(IMG_INIT_PNG);

    TTF_Font *inter = TTF_OpenFont("assets/ttf/Inter/Inter-VariableFont_opsz,wght.ttf", 16);
    TTF_SetFontStyle(inter, TTF_STYLE_NORMAL);
    TTF_SetFontOutline(inter, 0);
    TTF_SetFontKerning(inter, 1);
    TTF_SetFontHinting(inter, TTF_HINTING_NORMAL);
    TTF_SetFontWrappedAlign(inter, TTF_STYLE_NORMAL);

    if (!inter)
    {
        cout << "Error loading font: " << SDL_GetError() << endl;
        // End the program
        return 1;
    }

    SDL_Texture *shipTexture = IMG_LoadTexture(renderer, "assets/ship.png");
    SDL_Texture *stationTexture = IMG_LoadTexture(renderer, "assets/station.png");

    std::shared_ptr<EntityManager> entityManager = std::make_shared<EntityManager>();

    auto ship1 = std::make_shared<Ship>(vec2f(0, 0), 600, 100, 1.0, renderer);
    entityManager->addShip(ship1);
    auto ship2 = std::make_shared<Ship>(vec2f(0, 0), 600, 100, 1.0, renderer);
    entityManager->addShip(ship2);
    auto ship3 = std::make_shared<Ship>(vec2f(0, 0), 600, 100, 1.0, renderer);
    entityManager->addShip(ship3);
    // // auto ship3 = std::make_shared<Ship>(vec2f(0, 0), 600, 1000000);
    // // ships.push_back(ship3);
    // // auto ship4 = std::make_shared<Ship>(vec2f(0, 0), 600, 1000000);
    // // ships.push_back(ship4);
    // // auto ship5 = std::make_shared<Ship>(vec2f(0, 0), 600, 1000000);
    // // ships.push_back(ship5);
    // // auto ship6 = std::make_shared<Ship>(vec2f(0, 0), 600, 1000000);
    // // ships.push_back(ship6);
    // // auto ship7 = std::make_shared<Ship>(vec2f(0, 0), 600, 1000000);
    // // ships.push_back(ship7);
    // // auto ship8 = std::make_shared<Ship>(vec2f(0, 0), 600, 1000000);
    // // ships.push_back(ship8);

    auto mining_station = std::make_shared<ProductionStation>(vec2f(1000, 1000), "Silicon Miner Station 1", entityManager, renderer, inter);

    struct ProductionModule siliconProduction = {};
    siliconProduction.outputWares.push_back(wares::WareQuantity{Ware::Silicon, 100});
    siliconProduction.cycle_time = 1;
    siliconProduction.halted = false;

    mining_station->addProductionModule(siliconProduction);
    mining_station->setMaintenanceLevel(Ware::Silicon, 0);
    entityManager->addStation(mining_station);

    // auto mining_station_2 = std::make_shared<Station>(vec2f(800, 300), "Silicon Miner Station 2", renderer, inter);

    // struct ProductionModule siliconProduction2 = {};
    // siliconProduction2.outputWares.push_back({Ware::Silicon, 50});
    // siliconProduction2.cycle_time = 1;
    // siliconProduction2.halted = false;

    // mining_station_2->addProductionModule(siliconProduction2);
    // mining_station_2->setMaintenanceLevel(Ware::Silicon, 0);

    // stations.push_back(mining_station_2);

    // // auto mining_station_2 = std::make_shared<Station>(vec2f(800, 300), "Silicon Miner Station 2", renderer, inter);

    // // struct ProductionModule siliconProduction2 = {};
    // // siliconProduction2.outputWares.push_back({Ware::Silicon, 10});
    // // siliconProduction2.cycle_time = 10;
    // // siliconProduction2.halted = false;

    // // mining_station_2->addProductionModule(siliconProduction);
    // // mining_station_2->setMaintenanceLevel(Ware::Silicon, 0);
    // // mining_station->addShip(ship2);
    // // stations.push_back(mining_station_2);

    auto production_station = std::make_shared<ProductionStation>(vec2f(150, 200), "Silicon Wafer Production 1", entityManager, renderer, inter);

    struct ProductionModule siliconWaferProduction = {};
    siliconWaferProduction.inputWares.push_back({Ware::Silicon, 50});
    siliconWaferProduction.outputWares.push_back(wares::WareQuantity{Ware::SiliconWafers, 50});
    siliconWaferProduction.cycle_time = 1;

    production_station->addProductionModule(siliconWaferProduction);
    production_station->setMaintenanceLevel(Ware::Silicon, 1000);
    // production_station->setMaintenanceLevel(Ware::SiliconWafers, 100);
    production_station->reevaluateTradeOffers();

    production_station->addShip(ship1);

    entityManager->addStation(production_station);

    // auto production_station_2 = std::make_shared<Station>(vec2f(800, 500), "Silicon Wafer Production 2", renderer, inter);

    // struct ProductionModule siliconWaferProduction2 = {};
    // siliconWaferProduction2.inputWares.push_back({Ware::Silicon, 50});
    // siliconWaferProduction2.outputWares.push_back({Ware::SiliconWafers, 50});
    // siliconWaferProduction2.cycle_time = 1;

    // production_station_2->addProductionModule(siliconWaferProduction2);
    // production_station_2->setMaintenanceLevel(Ware::Silicon, 1000);
    // // production_station_2->setMaintenanceLevel(Ware::SiliconWafers, 100);
    // production_station_2->reevaluateTradeOffers();

    // production_station_2->addShip(ship2);

    // stations.push_back(production_station_2);

    // auto ship1 = std::make_shared<Ship>(vec2f(1500, 0), 100, 1000, 5, renderer);
    // entityManager->addShip(ship1);

    // auto trade_station = std::make_shared<Station>(vec2f(500, 500), "Trade Station 1", renderer, inter);

    // trade_station->setMaintenanceLevel(Ware::SiliconWafers, 1000);
    // trade_station->reevaluateTradeOffers();

    // stations.push_back(trade_station);

    vec2f camera = vec2f(0, 0);

    bool movingRight = false, movingLeft = false, movingUp = false, movingDown = false;

    bool quit = false;
    SDL_Event event;
    Uint64 NOW = SDL_GetPerformanceCounter();
    Uint64 LAST = 0;
    double deltaTime = 0;

    Uint64 FPS_TIMER = SDL_GetPerformanceCounter();
    int frames = 0;

    Uint64 lastTradeVolumeCheck = SDL_GetPerformanceCounter();

    while (!quit)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                quit = true;
            }
            if (event.type == SDL_KEYDOWN)
            {
                switch (event.key.keysym.sym)
                {
                case SDLK_RIGHT:
                    movingRight = true;
                    break;
                case SDLK_LEFT:
                    movingLeft = true;
                    break;
                case SDLK_UP:
                    movingUp = true;
                    break;
                case SDLK_DOWN:
                    movingDown = true;
                    break;
                }
            }
            if (event.type == SDL_KEYUP)
            {
                switch (event.key.keysym.sym)
                {
                case SDLK_RIGHT:
                    movingRight = false;
                    break;
                case SDLK_LEFT:
                    movingLeft = false;
                    break;
                case SDLK_UP:
                    movingUp = false;
                    break;
                case SDLK_DOWN:
                    movingDown = false;
                    break;
                }
            }
        }

        // SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        // SDL_RenderClear(renderer);

        // SDL_Surface *surface = TTF_RenderText_Blended(inter, "Sillicon Miner Station", {255, 255, 255, 255});
        // SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
        // int textWidth = surface->w;
        // int textHeight = surface->h;

        // SDL_Rect textRect = {0, 0, textWidth, textHeight};

        // SDL_FreeSurface(surface);

        // SDL_RenderCopy(renderer, texture, NULL, &textRect);

        // SDL_RenderPresent(renderer);

        // continue;

        frames++;

        LAST = NOW;
        NOW = SDL_GetPerformanceCounter();

        deltaTime = (double)((NOW - LAST) * 1000 / (double)SDL_GetPerformanceFrequency()) / 1000;

        // fps
        if (NOW - FPS_TIMER > SDL_GetPerformanceFrequency())
        {
            float fps = frames / static_cast<float>(NOW - FPS_TIMER) * SDL_GetPerformanceFrequency();
            cout << "FPS: " << fps << endl;
            FPS_TIMER = SDL_GetPerformanceCounter();
            frames = 0;
        }

        if (movingRight)
        {
            camera.x += PLAYER_SPEED * deltaTime;
        }
        if (movingLeft)
        {
            camera.x -= PLAYER_SPEED * deltaTime;
        }
        if (movingUp)
        {
            camera.y -= PLAYER_SPEED * deltaTime;
        }
        if (movingDown)
        {
            camera.y += PLAYER_SPEED * deltaTime;
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);

        for (auto &station : entityManager->getStations())
        {
            station->reevaluateTradeOffers();
            station->tick(deltaTime);
            station->render(camera);
        }

        for (auto &ship : entityManager->getShips())
        {
            ship->searchForTrade(entityManager->getStations());
            ship->tick(deltaTime);
            ship->render(camera);
        }

        struct MaxSellBuyOffersQuantities
        {
            int maxSellQuantity = 0;
            int maxBuyQuantity = 0;

            std::shared_ptr<Station> seller = nullptr;
            std::shared_ptr<Station> buyer = nullptr;
        };

        // every 5 seconds
        if (NOW - lastTradeVolumeCheck > SDL_GetPerformanceFrequency() * 5)
        {
            printf("Checking trade volume\n");
            lastTradeVolumeCheck = SDL_GetPerformanceCounter();

            std::map<Ware, MaxSellBuyOffersQuantities> maxGlobalSellBuyOffers;

            for (auto station : entityManager->getStations())
            {
                auto &sellOffers = station->getSellOffers();

                for (auto &sellOffer : sellOffers)
                {
                    if (maxGlobalSellBuyOffers.find(sellOffer.first) == maxGlobalSellBuyOffers.end())
                    {
                        MaxSellBuyOffersQuantities maxSellBuyOffersQuantities{sellOffer.second.quantity, 0, station, nullptr};

                        maxGlobalSellBuyOffers[sellOffer.first] = maxSellBuyOffersQuantities;
                        continue;
                    }

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
                        MaxSellBuyOffersQuantities maxSellBuyOffersQuantities{0, buyOffer.second.quantity, nullptr, station};

                        maxGlobalSellBuyOffers[buyOffer.first] = maxSellBuyOffersQuantities;
                        continue;
                    }

                    if (buyOffer.second.quantity > maxGlobalSellBuyOffers[buyOffer.first].maxBuyQuantity)
                    {
                        maxGlobalSellBuyOffers[buyOffer.first].maxBuyQuantity = buyOffer.second.quantity;
                        maxGlobalSellBuyOffers[buyOffer.first].buyer = station;
                    }
                }
            }

            auto &biggestTradeVolume = std::max(maxGlobalSellBuyOffers.begin(), maxGlobalSellBuyOffers.end(), [](const auto &a, const auto &b)
                                                { return std::min(a->second.maxSellQuantity, a->second.maxBuyQuantity) < std::min(b->second.maxSellQuantity, b->second.maxBuyQuantity); });

            int tradeVolume = std::min(biggestTradeVolume->second.maxSellQuantity, biggestTradeVolume->second.maxBuyQuantity);
            if (tradeVolume > 500)
            {
                // add one ship to the seller
                if (biggestTradeVolume->second.seller)
                {
                    std::shared_ptr<Station> station = entityManager->getStationById(biggestTradeVolume->second.seller->getId());
                    auto ship = std::make_shared<Ship>(biggestTradeVolume->second.seller->getPosition(), 600, 100, 1.0, renderer);
                    entityManager->addShip(ship);
                    station->addShip(ship);
                }
            }
        }

        SDL_RenderPresent(renderer);
        // SDL_Delay(1000);
    }

    SDL_DestroyTexture(shipTexture);
    SDL_DestroyTexture(stationTexture);
    SDL_DestroyRenderer(renderer);
    TTF_CloseFont(inter);
    TTF_Quit();
    IMG_Quit();
    SDL_DestroyWindow(window);
    SDL_Quit();

    // End the program
    return 0;
}

// #include <iostream>
// #include <chrono>
// #include <thread>

// #include "spdlog/spdlog.h"
// #include "spdlog/cfg/env.h"
// #include "spdlog/fmt/ostr.h"

// #include "ship.hpp"
// #include "station.hpp"

// int main()
// {
//     spdlog::cfg::load_env_levels();
//     spdlog::info("Initializing ship and station states");

//     std::vector<std::shared_ptr<Station>>
//         stations;
//     std::vector<std::shared_ptr<Ship>> ships;

//     auto ship1 = std::make_shared<Ship>(vec2f(0, 0), 10, 100);
//     ships.push_back(ship1);

//     auto mining_station = std::make_shared<Station>(vec2f(0, 0));

//     struct ProductionModule siliconProduction = {};
//     siliconProduction.outputWares.push_back({Ware::Silicon, 1000});
//     siliconProduction.cycle_time = 1;
//     siliconProduction.halted = false;

//     mining_station->addProductionModule(siliconProduction);
//     mining_station->setMaintenanceLevel(Ware::Silicon, 0);
//     mining_station->addShip(ship1);
//     stations.push_back(mining_station);

//     auto production_station = std::make_shared<Station>(vec2f(10, 10));

//     struct ProductionModule siliconWaferProduction = {};
//     siliconWaferProduction.inputWares.push_back({Ware::Silicon, 100});
//     siliconWaferProduction.outputWares.push_back({Ware::SiliconWafers, 50});
//     siliconWaferProduction.cycle_time = 10;

//     production_station->addProductionModule(siliconWaferProduction);
//     production_station->setMaintenanceLevel(Ware::Silicon, 1000);
//     production_station->reevaluateTradeOffers();

//     stations.push_back(production_station);

//     bool running = true;
//     while (running)
//     {
//         std::cout << "Mining station tick\n";

//         for (auto &ship : ships)
//         {
//             ship->searchForTrade(stations);
//         }

//         for (auto &station : stations)
//         {
//             station->tick(1);
//         }

//         std::this_thread::sleep_for(std::chrono::milliseconds(1000));
//     }

//     return 0;
// }