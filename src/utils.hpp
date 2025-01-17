#pragma once

#include <random>

namespace utils
{
    int generateId();

    extern std::random_device rd;
    extern std::mt19937 gen;
}