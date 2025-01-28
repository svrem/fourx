#include "productionStation.hpp"

void ProductionStation::addProductionModule(ProductionModule module)
{

    this->productionModules.push_back(module);

    for (auto &inputWare : module.inputWares)
    {
        if (this->inventory.find(inputWare.ware) == this->inventory.end())
        {
            this->inventory[inputWare.ware] = 0;
        }
    }
}
