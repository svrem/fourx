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
        int quantity;
    };

    struct Undock
    {
    };

    struct MoveToPosition
    {
        vec2f position;
    };

}

typedef std::variant<orders::DockAtStation, orders::TradeWithStation, orders::Undock, orders::MoveToPosition>
    ShipOrder;