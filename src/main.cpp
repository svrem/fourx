// #include <iostream>
#include <SDL2/SDL_image.h>

// #include "ball.hpp"
// #include "vec.hpp"

// #define SCREEN_WIDTH 1280
// #define SCREEN_HEIGHT 720

// // You shouldn't really use this statement, but it's fine for small programs
// using namespace std;

// // You must include the command line parameters for your main function to be recognized by SDL
// int main(int argc, char **args)
// {

//     // Pointers to our window and surface
//     SDL_Window *window = NULL;

//     // Initialize SDL. SDL_Init will return -1 if it fails.
//     if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
//     {
//         cout << "Error initializing SDL: " << SDL_GetError() << endl;
//         // End the program
//         return 1;
//     }

//     // Create our window
//     window = SDL_CreateWindow("Example", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

//     // Make sure creating the window succeeded
//     if (!window)
//     {
//         cout << "Error creating window: " << SDL_GetError() << endl;
//         // End the program
//         return 1;
//     }

//     // Get renderer
//     SDL_Renderer *gRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

//     if (!gRenderer)
//     {
//         cout << "Error creating renderer: " << SDL_GetError() << endl;
//         // End the program
//         return 1;
//     }

//     IMG_Init(IMG_INIT_PNG);

//     fourx::Ball ball{gRenderer, vec2i(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2), vec2i(-2, -4), 10};

//     bool quit = false;
//     SDL_Event event;
//     Uint64 NOW = SDL_GetPerformanceCounter();
//     Uint64 LAST = 0;
//     double deltaTime = 0;

//     while (!quit)
//     {
//         while (SDL_PollEvent(&event))
//         {
//             if (event.type == SDL_QUIT)
//             {
//                 quit = true;
//             }
//         }

//         LAST = NOW;
//         NOW = SDL_GetPerformanceCounter();

//         deltaTime = (double)((NOW - LAST) * 1000 / (double)SDL_GetPerformanceFrequency());

//         SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
//         SDL_RenderClear(gRenderer);

//         ball.update(deltaTime);
//         ball.draw();

//         SDL_RenderPresent(gRenderer);
//         cout << "Rendering" << endl;
//     }

//     // Destroy the window. This will also destroy the surface
//     SDL_DestroyWindow(window);

//     // Quit SDL
//     SDL_Quit();

//     // End the program
//     return 0;
// }

#include <iostream>
#include <chrono>
#include <thread>

#include "ship.hpp"
#include "station.hpp"

int main()
{
    std::vector<std::shared_ptr<Station>> stations;

    auto mining_station = std::make_shared<Station>(vec2f(0, 0));
    mining_station->setMaintenanceLevel(Ware::Silicon, 0);
    stations.push_back(mining_station);

    auto production_station = std::make_shared<Station>(vec2f(10, 10));

    struct ProductionModule siliconWaferProduction = {};
    siliconWaferProduction.inputWares.push_back({Ware::Silicon, 100});
    siliconWaferProduction.outputWares.push_back({Ware::SiliconWafers, 50});
    siliconWaferProduction.cycle_time = 10;

    production_station->addProductionModule(siliconWaferProduction);
    production_station->setMaintenanceLevel(Ware::Silicon, 1000);
    production_station->reevaluateTradeOffers();

    stations.push_back(production_station);

    bool running = true;
    while (running)
    {
        mining_station->addInventory(Ware::Silicon, 10);

        std::cout << "Mining station tick\n";

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    // station->addInventory(Ware::Silicon, 1000);

    // stations.push_back(station);

    // auto ship = std::make_unique<Ship>(vec2f(0, 0), 10, 100);

    // station->addShip(std::move(ship));

    return 0;
}