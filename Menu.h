#ifndef MENU_H
#define MENU_H

#include "Colony.h"
#include "Simulation.h"

#include <string>
#include <vector>

class Menu {
private:
    Colony* colony;
    Simulation* simulation;
    std::string dataDirectory;
    bool running;

    int promptInt(const std::string& prompt, int minValue, int maxValue) const;
    std::string promptLine(const std::string& prompt) const;
    void waitForEnter() const;

    int getNextColonistId() const;
    bool hasEnoughResources(const std::vector<ResourceCost>& costs) const;
    void applyResourceCosts(const std::vector<ResourceCost>& costs);

    void printEventBox(const std::vector<std::string>& lines, const std::string& colorCode) const;

    void showTitle() const;
    void showMainMenu() const;
    void showOverallStatus() const;

    void colonistMenu();
    void resourceMenu();
    void structureMenu();
    void disasterMenu();

    void displayColonistsTable(const std::vector<Colonist>& colonists) const;
    void displayResourcesTable(const std::vector<Resource>& resources) const;
    void displayStructuresTable(const std::vector<Structure>& structures) const;

public:
    Menu(Colony* colonyPtr, Simulation* simulationPtr, const std::string& dataDirectory);

    void run();
};

#endif
