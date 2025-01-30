#include "ui.hpp"

UI::UI(SDL_Renderer *renderer, TTF_Font *font) : m_Renderer(renderer), m_Font(font)
{
}

UI::~UI()
{
}

void UI::setTitle(std::string_view title)
{
    if (title == "")
    {
        if (m_TitleTexture != nullptr)
        {
            SDL_DestroyTexture(m_TitleTexture);
            m_TitleTexture = nullptr;
        }
        return;
    }

    TTF_SetFontStyle(m_Font, TTF_STYLE_BOLD);
    TTF_SetFontSize(m_Font, 16);
    SDL_Surface *titleSurface = TTF_RenderText_Blended(m_Font, title.data(), {255, 255, 255});
    m_TitleTexture = SDL_CreateTextureFromSurface(m_Renderer, titleSurface);
    m_TitleTextHeight = titleSurface->h;
    m_TitleTextWidth = titleSurface->w;
    SDL_FreeSurface(titleSurface);
}

SDL_Texture *UI::renderDataDisplay(UISupport::DataDisplay dataDisplay)
{
    const int width = 280;
    const int padding = 5;

    TTF_SetFontStyle(m_Font, TTF_STYLE_NORMAL);
    TTF_SetFontSize(m_Font, 16);

    std::vector<SDL_Texture *> textures;
    textures.reserve(dataDisplay.size());

    int totalHeight = 0;

    for (auto &data : dataDisplay)
    {
        SDL_Surface *nameSurface = TTF_RenderText_Blended(m_Font, data.name.data(), {255, 255, 255});
        SDL_Surface *valueSurface = TTF_RenderText_Blended(m_Font, data.value.data(), {255, 255, 255});

        if (nameSurface == nullptr || valueSurface == nullptr)
        {
            continue;
        }

        int height = std::max(nameSurface->h, valueSurface->h);
        totalHeight += height + padding;

        // combine nameSurface and valueSurface where nameSurface is on the left and valueSurface is on the right
        SDL_Surface *combinedSurface = SDL_CreateRGBSurface(0, width, std::max(nameSurface->h, valueSurface->h), 32, 0, 0, 0, 0);
        SDL_FillRect(combinedSurface, NULL, SDL_MapRGB(combinedSurface->format, 15, 15, 15));

        SDL_Rect dest;
        dest.x = 0;
        dest.y = 0;
        dest.w = nameSurface->w;
        dest.h = nameSurface->h;

        SDL_BlitSurface(nameSurface, NULL, combinedSurface, &dest);

        dest.x = width - valueSurface->w;
        dest.y = 0;
        dest.w = valueSurface->w;
        dest.h = valueSurface->h;

        SDL_BlitSurface(valueSurface, NULL, combinedSurface, &dest);

        SDL_Texture *texture = SDL_CreateTextureFromSurface(m_Renderer, combinedSurface);
        textures.push_back(texture);

        SDL_FreeSurface(nameSurface);
        SDL_FreeSurface(valueSurface);
        SDL_FreeSurface(combinedSurface);
    }

    SDL_Texture *combinedTexture = SDL_CreateTexture(m_Renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, totalHeight);

    SDL_SetRenderTarget(m_Renderer, combinedTexture);

    SDL_SetRenderDrawColor(m_Renderer, 15, 15, 15, 245);
    SDL_RenderClear(m_Renderer);

    int y = 0;

    for (auto &texture : textures)
    {
        SDL_Rect dest;
        dest.x = 0;
        dest.y = y;
        dest.w = width;
        dest.h = 0;

        SDL_QueryTexture(texture, NULL, NULL, NULL, &dest.h);
        SDL_RenderCopy(m_Renderer, texture, NULL, &dest);
        SDL_DestroyTexture(texture);

        y += dest.h + padding;
    }

    SDL_SetRenderTarget(m_Renderer, NULL);

    m_DataTextHeight = totalHeight;
    m_DataTextWidth = width;

    return combinedTexture;
}

void UI::setUIData(UISupport::UIData data)
{
    setTitle(data.title);

    m_DataTexture = renderDataDisplay(data.data);
}

void UI::render()
{

    int screenWidth, screenHeight;

    SDL_GetRendererOutputSize(m_Renderer, &screenWidth, &screenHeight);

    SDL_Rect dest;
    dest.x = 10;
    dest.y = screenHeight * 0.05;
    dest.w = 300;
    dest.h = screenHeight * 0.8;

    SDL_SetRenderDrawBlendMode(m_Renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(m_Renderer, 15, 15, 15, 245);
    SDL_RenderFillRect(m_Renderer, &dest);

    if (m_TitleTexture == nullptr)
    {
        return;
    }

    dest.x = 20;
    dest.y = screenHeight * 0.05 + 10;
    dest.w = m_TitleTextWidth;
    dest.h = m_TitleTextHeight;

    SDL_RenderCopy(m_Renderer, m_TitleTexture, NULL, &dest);

    if (m_DataTexture == nullptr)
    {
        return;
    }

    dest.x = 20;
    dest.y = screenHeight * 0.05 + 10 + m_TitleTextHeight + 10;
    dest.w = m_DataTextWidth;
    dest.h = m_DataTextHeight;

    SDL_RenderCopy(m_Renderer, m_DataTexture, NULL, &dest);
}

bool UI::checkForAndHandleMouesClick(Sint32 x, Sint32 y)
{
    return false;
}