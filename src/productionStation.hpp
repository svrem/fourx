#pragma once

#include "station.hpp"
#include "productionModule.hpp"

class ProductionStation : public Station
{
public:
    using Station::Station;

    void tick(float dt) override;
    void addProductionModule(ProductionModule module);

private:
    std::vector<ProductionModule> productionModules;

    void postUpdateInventory() override;
    void startNewProductionCycle(ProductionModule &productionModule);
};

namespace ProductionStationPreset
{
    inline std::shared_ptr<ProductionStation> createSiliconWaferProductionStation(vec2f position, std::string_view name, std::shared_ptr<EntityManager> entityManager, std::shared_ptr<UI> ui, SDL_Renderer *renderer, TTF_Font *font)
    {
        auto productionStation = std::make_shared<ProductionStation>(position, name, entityManager, ui, renderer, font);
        productionStation->addProductionModule(ProductionModulePreset::createSiliconWaferProduction());
        productionStation->setMaintenanceLevel(Ware::Silicon, 1000);
        productionStation->setMaintenanceLevel(Ware::SiliconWafers, 0);
        return productionStation;
    }

    inline std::shared_ptr<ProductionStation> createSiliconProductionStation(vec2f position, std::string_view name, std::shared_ptr<EntityManager> entityManager, std::shared_ptr<UI> ui, SDL_Renderer *renderer, TTF_Font *font)
    {
        auto productionStation = std::make_shared<ProductionStation>(position, name, entityManager, ui, renderer, font);
        productionStation->addProductionModule(ProductionModulePreset::createSiliconProduction());
        productionStation->setMaintenanceLevel(Ware::Silicon, 0);
        return productionStation;
    }

}