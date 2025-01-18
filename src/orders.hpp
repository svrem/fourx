#pragma once

#include <variant>
#include <memory>

#include "wares.hpp"

class Station;

namespace orders
{

    struct DockAtStation
    {
        std::shared_ptr<Station> station;
    };

    struct TradeWithStation
    {
        std::shared_ptr<Station> station;
        wares::TradeType type;
        wares::Ware ware;
        float quantity;
    };

    struct Undock
    {
    };

}

typedef std::variant<orders::DockAtStation, orders::TradeWithStation, orders::Undock>
    ShipOrder;