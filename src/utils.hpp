#pragma once

#include "entityManager.hpp"

#include "SDL2/SDL.h"

#include <memory>
#include <random>

#include <random>

namespace utils
{
    int generateId();

    extern std::random_device rd;
    extern std::mt19937 gen;
}

void shipPurchaseCheck(std::shared_ptr<EntityManager> entityManager, Uint64 &lastTradeVolumeCheck);