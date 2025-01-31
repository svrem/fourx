

#include "game.hpp"

#include "spdlog/spdlog.h"
#include "spdlog/cfg/env.h"

int main()
{
    // Initialize spdlog
    spdlog::cfg::load_env_levels();
    spdlog::info("Initializing ship and station states");

    Game game;
    game.run();

    return 0;
}