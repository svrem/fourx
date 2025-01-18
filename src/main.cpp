

#include "ship.hpp"
#include "station.hpp"
#include "vec.hpp"

#include <SDL2/SDL_image.h>
#include "spdlog/spdlog.h"
#include "spdlog/cfg/env.h"
#include "spdlog/fmt/ostr.h"

#include <iostream>
#include <vector>
#include <memory>

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

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

    SDL_Texture *shipTexture = IMG_LoadTexture(renderer, "assets/ship.png");
    SDL_Texture *stationTexture = IMG_LoadTexture(renderer, "assets/station.png");

    std::vector<std::shared_ptr<Station>>
        stations;
    std::vector<std::shared_ptr<Ship>> ships;

    auto ship1 = std::make_shared<Ship>(vec2f(0, 0), 100, 1000000);
    ships.push_back(ship1);

    auto mining_station = std::make_shared<Station>(vec2f(300, 400), renderer);

    struct ProductionModule siliconProduction = {};
    siliconProduction.outputWares.push_back({Ware::Silicon, 10});
    siliconProduction.cycle_time = 1;
    siliconProduction.halted = false;

    mining_station->addProductionModule(siliconProduction);
    mining_station->setMaintenanceLevel(Ware::Silicon, 0);
    // mining_station->addShip(ship1);
    stations.push_back(mining_station);

    auto production_station = std::make_shared<Station>(vec2f(150, 200), renderer);

    struct ProductionModule siliconWaferProduction = {};
    siliconWaferProduction.inputWares.push_back({Ware::Silicon, 100});
    siliconWaferProduction.outputWares.push_back({Ware::SiliconWafers, 50});
    siliconWaferProduction.cycle_time = 10;

    production_station->addProductionModule(siliconWaferProduction);
    production_station->setMaintenanceLevel(Ware::Silicon, 1000);
    production_station->reevaluateTradeOffers();

    production_station->addShip(ship1);

    stations.push_back(production_station);

    bool quit = false;
    SDL_Event event;
    Uint64 NOW = SDL_GetPerformanceCounter();
    Uint64 LAST = 0;
    double deltaTime = 0;

    Uint64 FPS_TIMER = SDL_GetPerformanceCounter();
    int frames = 0;

    while (!quit)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                quit = true;
            }
        }

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

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);

        for (auto &station : stations)
        {
            station->tick(deltaTime);
            station->render();
        }

        for (auto &ship : ships)
        {
            ship->searchForTrade(stations);
            ship->tick(deltaTime);
            ship->render(renderer);
        }

        // std::vector<SDL_Vertex> shipVertices =
        //     {
        //         {
        //             SDL_FPoint{400, 150},
        //             SHIP_COLOR,
        //             SDL_FPoint{0},
        //         },
        //         {
        //             SDL_FPoint{406, 162},
        //             SHIP_COLOR,
        //             SDL_FPoint{0},
        //         },
        //         {
        //             SDL_FPoint{394, 162},
        //             SHIP_COLOR,
        //             SDL_FPoint{0},
        //         },
        //     };

        // SDL_RenderGeometry(renderer, NULL, shipVertices.data(), shipVertices.size(), NULL, 0);

        // SDL_RenderCopy(renderer, ship, NULL, &shipRect);
        // SDL_RenderCopy(renderer, stationTexture, NULL, &stationRect);

        SDL_RenderPresent(renderer);
    }

    // Destroy the window. This will also destroy the surface
    SDL_DestroyWindow(window);

    // Quit SDL
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