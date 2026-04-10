#include "Simulation.h"

#include <algorithm>
#include <cmath>
#include <sstream>

Simulation::Simulation(Colony* colonyPtr)
    : colony(colonyPtr), randomEngine(std::random_device{}()) {}

int Simulation::randomInt(int minValue, int maxValue) {
    std::uniform_int_distribution<int> distribution(minValue, maxValue);
    return distribution(randomEngine);
}

Resource* Simulation::getResource(const std::string& resourceName) {
    if (colony == nullptr) {
        return nullptr;
    }
    return colony->findResourceByName(resourceName);
}

void Simulation::applyStructureProduction(std::vector<std::string>& log) {
    std::vector<Structure>& structures = colony->getStructures();
    for (Structure& structure : structures) {
        if (!structure.isOperational()) {
            continue;
        }

        std::vector<ResourceCost> production = structure.getDailyProduction();
        for (const ResourceCost& produced : production) {
            Resource* resource = getResource(produced.resourceName);
            if (resource != nullptr && produced.amount > 0) {
                resource->addQuantity(produced.amount);
                std::ostringstream line;
                line << "  + " << structure.getName() << " produced "
                     << produced.amount << " " << produced.resourceName << ".";
                log.push_back(line.str());
            }
        }
    }
}

void Simulation::applyDailyConsumption(std::vector<std::string>& log) {
    int colonistCount = colony->getAliveColonistCount();
    if (colonistCount <= 0) {
        return;
    }

    for (Resource& resource : colony->getResources()) {
        int baseConsumption = static_cast<int>(std::ceil(resource.getDailyConsumptionRate() * colonistCount));
        if (resource.getResourceName() == "Energy") {
            baseConsumption += static_cast<int>(colony->getStructures().size()) * 2;
        }

        int consumed = resource.consumeQuantity(baseConsumption);
        std::ostringstream line;
        line << "  - Consumed " << consumed << " " << resource.getResourceName() << ".";
        log.push_back(line.str());

        if (consumed < baseConsumption) {
            std::ostringstream shortage;
            shortage << "  ! Shortage: Needed " << baseConsumption << " "
                     << resource.getResourceName() << ", but only " << consumed << " was available.";
            log.push_back(shortage.str());
        }
    }
}

void Simulation::applyResourceHealthEffects(std::vector<std::string>& log) {
    Resource* food = getResource("Food");
    Resource* oxygen = getResource("Oxygen");
    Resource* energy = getResource("Energy");

    int aliveColonists = colony->getAliveColonistCount();
    if (aliveColonists <= 0) {
        return;
    }

    int foodPenalty = 0;
    int oxygenPenalty = 0;
    int energyPenalty = 0;

    if (food != nullptr) {
        if (food->getQuantity() <= 0) {
            foodPenalty = 24;
        } else if (food->getQuantity() < aliveColonists) {
            foodPenalty = 12;
        }
    }

    if (oxygen != nullptr) {
        if (oxygen->getQuantity() <= 0) {
            oxygenPenalty = 35;
        } else if (oxygen->getQuantity() < (aliveColonists * 2)) {
            oxygenPenalty = 18;
        }
    }

    if (energy != nullptr) {
        if (energy->getQuantity() <= 0) {
            energyPenalty = 12;
        } else if (energy->getQuantity() < aliveColonists) {
            energyPenalty = 6;
        }
    }

    int totalPenalty = foodPenalty + oxygenPenalty + energyPenalty;
    if (totalPenalty <= 0) {
        return;
    }

    for (Colonist& colonist : colony->getColonists()) {
        colonist.damage(totalPenalty);
    }

    if (foodPenalty > 0) {
        log.push_back("  ! Food shortage detected. Colonists lose health rapidly.");
    }
    if (oxygenPenalty > 0) {
        log.push_back("  ! Oxygen is dangerously low. The air has become uncomfortable.");
    }
    if (energyPenalty > 0) {
        log.push_back("  ! Energy grid instability affects life support systems.");
    }
}

void Simulation::healIfWellSupplied(std::vector<std::string>& log) {
    Resource* food = getResource("Food");
    Resource* oxygen = getResource("Oxygen");
    Resource* energy = getResource("Energy");

    int aliveColonists = colony->getAliveColonistCount();
    if (aliveColonists <= 0 || food == nullptr || oxygen == nullptr || energy == nullptr) {
        return;
    }

    if (food->getQuantity() > aliveColonists * 6 &&
        oxygen->getQuantity() > aliveColonists * 8 &&
        energy->getQuantity() > aliveColonists * 5) {
        for (Colonist& colonist : colony->getColonists()) {
            colonist.heal(4);
        }
        log.push_back("  + Resources were abundant today. Colonists recovered some health.");
    }
}

void Simulation::ageStructures(std::vector<std::string>& log) {
    for (Structure& structure : colony->getStructures()) {
        if (!structure.isOperational()) {
            continue;
        }

        int wear = randomInt(0, 2);
        if (wear > 0) {
            structure.damage(wear);
            std::ostringstream line;
            line << "  - " << structure.getName() << " lost " << wear << " durability from daily wear.";
            log.push_back(line.str());
        }

        if (!structure.isOperational()) {
            std::ostringstream collapse;
            collapse << "  X ALERT: " << structure.getName() << " collapsed!";
            log.push_back(collapse.str());
        }
    }
}

void Simulation::applyEventByType(int eventType, std::vector<std::string>& log) {
    std::vector<Colonist>& colonists = colony->getColonists();
    std::vector<Structure>& structures = colony->getStructures();

    switch (eventType) {
    case 0: {
        log.push_back("  ** METEOR STRIKE ** A debris storm slams the colony perimeter!");

        if (!structures.empty()) {
            int hits = std::min(static_cast<int>(structures.size()), randomInt(1, 3));
            for (int i = 0; i < hits; ++i) {
                int index = randomInt(0, static_cast<int>(structures.size()) - 1);
                int damageAmount = randomInt(12, 36);
                structures[static_cast<std::size_t>(index)].damage(damageAmount);

                std::ostringstream line;
                line << "    Structure hit: " << structures[static_cast<std::size_t>(index)].getName()
                     << " took " << damageAmount << " damage.";
                log.push_back(line.str());
            }
        }

        if (!colonists.empty()) {
            int injured = std::min(static_cast<int>(colonists.size()), randomInt(1, 3));
            for (int i = 0; i < injured; ++i) {
                int index = randomInt(0, static_cast<int>(colonists.size()) - 1);
                int damageAmount = randomInt(8, 20);
                colonists[static_cast<std::size_t>(index)].damage(damageAmount);

                std::ostringstream line;
                line << "    Colonist injured: " << colonists[static_cast<std::size_t>(index)].getName()
                     << " lost " << damageAmount << " health.";
                log.push_back(line.str());
            }
        }
        break;
    }
    case 1: {
        log.push_back("  ** ILLNESS OUTBREAK ** A contagious fever spreads through living quarters.");
        if (!colonists.empty()) {
            int affected = std::max(1, static_cast<int>(colonists.size() / 2));
            for (int i = 0; i < affected; ++i) {
                int index = randomInt(0, static_cast<int>(colonists.size()) - 1);
                int damageAmount = randomInt(7, 16);
                colonists[static_cast<std::size_t>(index)].damage(damageAmount);
            }
            log.push_back("    Medical teams are overwhelmed as multiple colonists lose health.");
        }
        break;
    }
    case 2: {
        log.push_back("  ** SANDSTORM ** Fine abrasive dust tears across the outpost.");

        Resource* energy = getResource("Energy");
        Resource* food = getResource("Food");

        if (energy != nullptr) {
            int lost = energy->consumeQuantity(randomInt(18, 34));
            std::ostringstream line;
            line << "    Sandstorm disrupted solar intake. Energy lost: " << lost << ".";
            log.push_back(line.str());
        }

        if (food != nullptr) {
            int lost = food->consumeQuantity(randomInt(6, 15));
            std::ostringstream line;
            line << "    Supplies contaminated. Food lost: " << lost << ".";
            log.push_back(line.str());
        }

        if (!structures.empty()) {
            int index = randomInt(0, static_cast<int>(structures.size()) - 1);
            int damageAmount = randomInt(5, 14);
            structures[static_cast<std::size_t>(index)].damage(damageAmount);
            std::ostringstream line;
            line << "    " << structures[static_cast<std::size_t>(index)].getName() << " was scratched by debris ("
                 << damageAmount << " damage).";
            log.push_back(line.str());
        }
        break;
    }
    case 3: {
        log.push_back("  ** POWER SURGE ** Reactor feedback cascade detected!");

        Resource* energy = getResource("Energy");
        if (energy != nullptr) {
            int lost = energy->consumeQuantity(randomInt(10, 28));
            std::ostringstream line;
            line << "    Emergency venting consumed " << lost << " energy.";
            log.push_back(line.str());
        }

        if (!structures.empty()) {
            int index = randomInt(0, static_cast<int>(structures.size()) - 1);
            int damageAmount = randomInt(8, 18);
            structures[static_cast<std::size_t>(index)].damage(damageAmount);
            std::ostringstream line;
            line << "    " << structures[static_cast<std::size_t>(index)].getName()
                 << " overloaded and took " << damageAmount << " damage.";
            log.push_back(line.str());
        }
        break;
    }
    case 4: {
        log.push_back("  ** ALIEN SIGHTING ** A mysterious caravan leaves behind rare salvage.");

        Resource* food = getResource("Food");
        Resource* oxygen = getResource("Oxygen");
        Resource* energy = getResource("Energy");

        if (food != nullptr) {
            food->addQuantity(randomInt(10, 20));
        }
        if (oxygen != nullptr) {
            oxygen->addQuantity(randomInt(12, 24));
        }
        if (energy != nullptr) {
            energy->addQuantity(randomInt(14, 28));
        }

        for (Colonist& colonist : colonists) {
            colonist.heal(3);
        }

        log.push_back("    Morale skyrockets. Supplies improved and colonists feel inspired.");
        break;
    }
    default:
        break;
    }
}

std::vector<std::string> Simulation::advanceDay() {
    std::vector<std::string> log;

    colony->incrementDay();

    std::ostringstream header;
    header << "=== DAY " << colony->getDayNumber() << " REPORT ===";
    log.push_back(header.str());

    log.push_back("- Structure Production");
    applyStructureProduction(log);

    log.push_back("- Daily Consumption");
    applyDailyConsumption(log);

    log.push_back("- Life Support Impact");
    applyResourceHealthEffects(log);
    healIfWellSupplied(log);

    log.push_back("- Structural Wear");
    ageStructures(log);

    if (randomInt(1, 100) <= 45) {
        int eventType = randomInt(0, 4);
        log.push_back("- Random Event Triggered");
        applyEventByType(eventType, log);
    } else {
        log.push_back("- Random Event Triggered");
        log.push_back("  Calm day. No major disasters occurred.");
    }

    std::vector<std::string> removedNames;
    int removedCount = colony->removeDeadColonists(&removedNames);
    if (removedCount > 0) {
        for (const std::string& name : removedNames) {
            std::ostringstream line;
            line << "  X Tragic loss: " << name << " did not survive the day.";
            log.push_back(line.str());
        }
    }

    if (colony->isColonyThriving()) {
        log.push_back("  *** The colony is thriving. Command morale is at an all-time high! ***");
    }

    if (colony->isColonyFailed()) {
        log.push_back("  !!! COLONY FAILURE: Oxygen and life support have reached catastrophic levels. !!!");
    }

    return log;
}

std::vector<std::string> Simulation::triggerRandomEvent() {
    std::vector<std::string> log;
    log.push_back("=== MANUAL EVENT TRIGGER ===");

    int eventType = randomInt(0, 4);
    applyEventByType(eventType, log);

    std::vector<std::string> removedNames;
    colony->removeDeadColonists(&removedNames);
    for (const std::string& name : removedNames) {
        std::ostringstream line;
        line << "  X Tragic loss: " << name << " did not survive the event.";
        log.push_back(line.str());
    }

    return log;
}

std::vector<std::string> Simulation::triggerSpecificEvent(int eventType) {
    std::vector<std::string> log;
    log.push_back("=== FORCED EVENT ===");
    applyEventByType(eventType, log);

    std::vector<std::string> removedNames;
    colony->removeDeadColonists(&removedNames);
    for (const std::string& name : removedNames) {
        std::ostringstream line;
        line << "  X Tragic loss: " << name << " did not survive the event.";
        log.push_back(line.str());
    }

    return log;
}

std::vector<std::string> Simulation::simulateResourceUsage() {
    std::vector<std::string> log;
    log.push_back("=== RESOURCE USAGE SIMULATION (NO DAY ADVANCE) ===");

    applyDailyConsumption(log);
    applyResourceHealthEffects(log);
    healIfWellSupplied(log);

    std::vector<std::string> removedNames;
    colony->removeDeadColonists(&removedNames);
    for (const std::string& name : removedNames) {
        std::ostringstream line;
        line << "  X Critical loss: " << name << " died during resource simulation.";
        log.push_back(line.str());
    }

    return log;
}
