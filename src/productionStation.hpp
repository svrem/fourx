#pragma once

#include "station.hpp"

class ProductionStation : public Station
{
public:
    using Station::Station;

    void addProductionModule(ProductionModule module);
};