#include <iostream>
#include <SDL2/SDL_image.h>

#include "vec.hpp"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

// You shouldn't really use this statement, but it's fine for small programs
using namespace std;

// You must include the command line parameters for your main function to be recognized by SDL
int main(int argc, char **args)
{

    // Pointers to our window and surface
    SDL_Window *window = NULL;

    // Initialize SDL. SDL_Init will return -1 if it fails.
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        cout << "Error initializing SDL: " << SDL_GetError() << endl;
        // End the program
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
    SDL_Renderer *gRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (!gRenderer)
    {
        cout << "Error creating renderer: " << SDL_GetError() << endl;
        // End the program
        return 1;
    }

    IMG_Init(IMG_INIT_PNG);

    bool quit = false;
    SDL_Event event;
    Uint64 NOW = SDL_GetPerformanceCounter();
    Uint64 LAST = 0;
    double deltaTime = 0;

    while (!quit)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                quit = true;
            }
        }

        LAST = NOW;
        NOW = SDL_GetPerformanceCounter();

        deltaTime = (double)((NOW - LAST) * 1000 / (double)SDL_GetPerformanceFrequency());

        SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderClear(gRenderer);

        SDL_RenderPresent(gRenderer);
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