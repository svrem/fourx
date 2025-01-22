#pragma once

#include <map>
#include <string>

namespace wares
{

    enum Ware
    {
        HullParts,
        EnergyCells,
        Ore,
        SiliconWafers,
        Silicon,
    };

    struct WareDetails
    {
        float density;
        float min_price;
        float max_price;
        std::string name;
    };

    const std::map<Ware, WareDetails> wareDetails = {
        {HullParts, {1.0, 10.0, 20.0, "Hull Parts"}},
        {EnergyCells, {0.5, 5.0, 10.0, "Energy Cells"}},
        {Ore, {2.0, 1.0, 2.0, "Ore"}},
        {SiliconWafers, {0.1, 1.0, 1.0, "Silicon Wafers"}},
        {Silicon, {0.5, 1.0, 5.0, "Silicon"}},
    };

    enum class TradeType
    {
        Buy,
        Sell
    };

    struct Offer
    {
        float price;
        float quantity;
    };

    struct WareQuantity
    {
        Ware ware;
        int quantity;
    };
}