#ifndef SIMULATION_H
#define SIMULATION_H

#include "Colony.h"

#include <random>
#include <string>
#include <vector>

class Simulation {
private:
    Colony* colony;
    std::mt19937 randomEngine;

    int randomInt(int minValue, int maxValue);
    Resource* getResource(const std::string& resourceName);

    void applyStructureProduction(std::vector<std::string>& log);
    void applyDailyConsumption(std::vector<std::string>& log);
    void applyResourceHealthEffects(std::vector<std::string>& log);
    void healIfWellSupplied(std::vector<std::string>& log);
    void ageStructures(std::vector<std::string>& log);
    void applyEventByType(int eventType, std::vector<std::string>& log);

public:
    explicit Simulation(Colony* colonyPtr);

    std::vector<std::string> advanceDay();
    std::vector<std::string> triggerRandomEvent();
    std::vector<std::string> triggerSpecificEvent(int eventType);
    std::vector<std::string> simulateResourceUsage();
};

#endif
