#include "productionStation.hpp"

void ProductionStation::addProductionModule(ProductionModule module)
{

    this->productionModules.push_back(module);
    this->startNewProductionCycle(productionModules.back());

    for (auto &inputWare : module.inputWares)
    {
        if (this->inventory.find(inputWare.ware) == this->inventory.end())
        {
            this->inventory[inputWare.ware] = 0;
        }
    }
}

void ProductionStation::startNewProductionCycle(ProductionModule &productionModule)
{
    productionModule.halted = false;
    for (auto &inputWare : productionModule.inputWares)
    {
        if (inventory[inputWare.ware] < inputWare.quantity)
        {
            productionModule.halted = true;
            break;
        }
    }

    if (productionModule.halted)
    {
        // not enough input wares
        productionModule.current_cycle_time = 0;
        return;
    }

    for (auto &inputWare : productionModule.inputWares)
    {
        this->updateInventory(inputWare.ware, -inputWare.quantity);
    }
}

void ProductionStation::tick(float dt)
{

    for (auto &productionModule : this->productionModules)
    {
        if (productionModule.halted)
            continue;

        productionModule.current_cycle_time += dt;

        if (productionModule.current_cycle_time < productionModule.cycle_time)
            continue;

        for (auto &outputWare : productionModule.outputWares)
        {
            if (std::holds_alternative<wares::WareQuantity>(outputWare))
            {
                auto wareQuantity = std::get<wares::WareQuantity>(outputWare);
                this->updateInventory(wareQuantity.ware, wareQuantity.quantity);
                continue;
            }
            else if (std::holds_alternative<wares::ShipOrder>(outputWare))
            {
                auto shipOrder = std::get<wares::ShipOrder>(outputWare);
                auto ship = std::make_shared<Ship>(this->m_Position, shipOrder.maxSpeed, shipOrder.cargoCapacity, shipOrder.weaponAttack, m_Renderer);

                continue;
            }
        }

        productionModule.current_cycle_time -= productionModule.cycle_time;

        // start new cycle
        startNewProductionCycle(productionModule);
    }
}

void ProductionStation::postUpdateInventory()
{
    for (auto &productionModule : this->productionModules)
    {
        if (productionModule.halted)
        {
            startNewProductionCycle(productionModule);
        }
    }
}