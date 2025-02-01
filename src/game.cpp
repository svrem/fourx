#include "game.hpp"
#include "productionStation.hpp"
#include "productionModule.hpp"
#include "ship.hpp"
#include "warfStation.hpp"
#include "ui.hpp"
#include "utils.hpp"

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_ttf.h"

#include <iostream>
#include <stdexcept>

Game::Game()
{
    initializeSDL();
    initializeEntities();
}

Game::~Game()
{
    TTF_CloseFont(m_Font);
    TTF_Quit();
    SDL_DestroyRenderer(m_Renderer);
    SDL_DestroyWindow(m_Window);
    SDL_Quit();
}

void Game::initializeSDL()
{

    // Pointers to our window and surface

    if (TTF_Init() < 0)
    {
        throw std::runtime_error(std::string("TTF_Init: ") + TTF_GetError());
    }

    // Initialize SDL. SDL_Init will return -1 if it fails.
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        throw std::runtime_error(std::string("SDL_Init failed: ") + SDL_GetError());
    }

    if (SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1") == SDL_FALSE)
    {
        throw std::runtime_error("Warning: Linear texture filtering not enabled!");
    }

    // Create our window
    m_Window = SDL_CreateWindow("Example", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    // Make sure creating the window succeeded
    if (!m_Window)
    {
        throw std::runtime_error(std::string("Failed to create window: ") + SDL_GetError());
    }

    // Get renderer
    m_Renderer = SDL_CreateRenderer(m_Window, -1, SDL_RENDERER_ACCELERATED);

    if (!m_Renderer)
    {
        throw std::runtime_error(std::string("Renderer could not be created! SDL Error: ") + SDL_GetError());
    }

    IMG_Init(IMG_INIT_PNG);

    m_Font = TTF_OpenFont("assets/ttf/Inter/Inter-VariableFont_opsz,wght.ttf", 16);

    if (!m_Font)
    {
        throw std::runtime_error(std::string("Failed to load font: ") + TTF_GetError());
    }

    TTF_SetFontStyle(m_Font, TTF_STYLE_NORMAL);
    TTF_SetFontOutline(m_Font, 0);
    TTF_SetFontKerning(m_Font, 1);
    TTF_SetFontHinting(m_Font, TTF_HINTING_NORMAL);
    TTF_SetFontWrappedAlign(m_Font, TTF_STYLE_NORMAL);

    if (!m_Font)
    {
        throw std::runtime_error(std::string("Failed to load font: ") + TTF_GetError());
    }
}

void Game::initializeEntities()
{
    m_UI = std::make_shared<UI>(m_Renderer, m_Font);
    m_EntityManager = std::make_shared<EntityManager>();

    for (uint i = 0; i < 1000; i++)
    {
        float x = static_cast<float>(utils::gen() % 50000) - 25000.0f;
        float y = static_cast<float>(utils::gen() % 50000) - 25000.0f;
        auto station = ProductionStationPreset::createSiliconWaferProductionStation(vec2f(x, y), "Silicon Wafer Production " + std::to_string(i), m_EntityManager, m_UI, m_Renderer, m_Font);
        m_EntityManager->addStation(station);
    }

    for (uint i = 0; i < 1000; i++)
    {
        float x = static_cast<float>(utils::gen() % 50000) - 25000.0f;
        float y = static_cast<float>(utils::gen() % 50000) - 25000.0f;

        auto ship = ShipPreset::createFreighter(vec2f(x, y), m_Renderer);
        auto station = ProductionStationPreset::createSiliconProductionStation(vec2f(x, y), "Silicon Production " + std::to_string(i), m_EntityManager, m_UI, m_Renderer, m_Font);

        station->addShip(ship);
        m_EntityManager->addShip(ship);

        m_EntityManager->addStation(station);
    }

    auto warfStation1 = std::make_shared<WarfStation>(vec2f(500, 400), "Warf Station 1", m_EntityManager, m_UI, m_Renderer, m_Font);
    warfStation1->setMaintenanceLevel(Ware::SiliconWafers, 100000);

    auto ship = ShipPreset::createFreighter(vec2f(500, 500), m_Renderer);
    m_EntityManager->addShip(ship);

    warfStation1->addShip(ship);

    m_EntityManager->addWarfStation(warfStation1);
}

void Game::run()
{
    vec2f camera = vec2f(0, 0);
    float zoomLevel = 1.0f;

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

            if (event.type == SDL_MOUSEBUTTONUP)
            {
                for (auto &station : m_EntityManager->getStations())
                {
                    station->deselect();
                }

                for (auto &station : m_EntityManager->getStations())
                {
                    station->checkForAndHandleMouseClick(camera, event.button.x, event.button.y);
                }
            }

            if (event.type == SDL_MOUSEWHEEL)
            {
                zoomLevel += event.wheel.y * 0.1f;
                if (zoomLevel < 0.1f)
                {
                    zoomLevel = 0.1f;
                }
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
            printf("FPS: %f\n", fps);
            FPS_TIMER = SDL_GetPerformanceCounter();
            frames = 0;
        }

        if (movingRight)
        {
            camera.x += PLAYER_SPEED * deltaTime * 1 / zoomLevel;
        }
        if (movingLeft)
        {
            camera.x -= PLAYER_SPEED * deltaTime * 1 / zoomLevel;
        }
        if (movingUp)
        {
            camera.y -= PLAYER_SPEED * deltaTime * 1 / zoomLevel;
        }
        if (movingDown)
        {
            camera.y += PLAYER_SPEED * deltaTime * 1 / zoomLevel;
        }

        SDL_SetRenderDrawColor(m_Renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(m_Renderer);

        int screenWidth, screenHeight;
        SDL_GetRendererOutputSize(m_Renderer, &screenWidth, &screenHeight);
        vec2f zoomCenter = vec2f(screenWidth / 2, screenHeight / 2);

        for (auto &station : m_EntityManager->getStations())
        {
            station->reevaluateTradeOffers();
            station->tick(deltaTime);
            station->render(camera, zoomLevel, zoomCenter);
        }

        for (auto &ship : m_EntityManager->getShips())
        {
            ship->searchForTrade(m_EntityManager->getStations(), deltaTime);
            ship->tick(deltaTime);
            ship->render(camera, zoomLevel, zoomCenter);
        }

        // every 5 seconds
        if (NOW - lastTradeVolumeCheck > SDL_GetPerformanceFrequency() * 5)
        {
            shipPurchaseCheck(m_EntityManager, lastTradeVolumeCheck);
        }

        m_UI->render();

        SDL_RenderPresent(m_Renderer);
        // SDL_Delay(1000);
    }
}