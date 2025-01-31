#pragma once

#include "entityManager.hpp"
#include "ui.hpp"
#include "vec.hpp"

#include <memory>

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

#define PLAYER_SPEED 500

class Game
{
public:
    Game();
    ~Game();

    void run();

private:
    void initializeEntities();
    void initializeSDL();

    SDL_Window *m_Window = nullptr;
    SDL_Renderer *m_Renderer = nullptr;
    TTF_Font *m_Font = nullptr;

    std::shared_ptr<EntityManager> m_EntityManager = nullptr;
    std::shared_ptr<UI> m_UI = nullptr;
    vec2f m_Camera;
};