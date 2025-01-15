// #include <iostream>
// #include <SDL2/SDL_image.h>

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

#include "ship.hpp"
#include "station.hpp"

int main()
{
    std::vector<std::shared_ptr<Station>> stations;

    auto station = std::make_shared<Station>(vec2f(0, 0));

    struct ProductionModule siliconWaferProduction = {};
    siliconWaferProduction.inputWares.push_back({Ware::Silicon, 100});
    siliconWaferProduction.outputWares.push_back({Ware::SiliconWafers, 50});
    siliconWaferProduction.cycle_time = 10;
    station->addProductionModule(siliconWaferProduction);

    station->addInventory(Ware::Silicon, 1000);

    station->tick(10);
    station->tick(10);

    station->__debug_print_inventory();

    stations.push_back(station);

    auto ship = std::make_unique<Ship>(vec2f(0, 0), 10, 100);

    station->addShip(std::move(ship));

    if (stations[0] == station)
    {
        std::cout << "Station is the same" << std::endl;
    }

    return 0;
}