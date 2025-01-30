#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <string>
#include <string_view>
#include <vector>

namespace UISupport
{
    struct SingleDataDisplay
    {
        std::string name;
        std::string value;
    };
    using DataDisplay = std::vector<SingleDataDisplay>;

    struct UIData
    {
        std::string_view title;
        DataDisplay data;
    };
};

class UI
{
public:
    UI(SDL_Renderer *renderer, TTF_Font *font);
    ~UI();

    void render();
    bool checkForAndHandleMouesClick(Sint32 x, Sint32 y);

    void setUIData(UISupport::UIData data);

private:
    void setTitle(std::string_view title);
    SDL_Texture *renderDataDisplay(UISupport::DataDisplay data);

    SDL_Renderer *m_Renderer;
    TTF_Font *m_Font;

    SDL_Texture *m_TitleTexture = nullptr;
    int m_TitleTextWidth, m_TitleTextHeight;

    SDL_Texture *m_DataTexture = nullptr;
    int m_DataTextWidth, m_DataTextHeight;
};