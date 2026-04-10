#include "Menu.h"

#include "FileManager.h"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>

namespace {
const std::string CLR_RESET = "\033[0m";
const std::string CLR_CYAN = "\033[36m";
const std::string CLR_BLUE = "\033[34m";
const std::string CLR_GREEN = "\033[32m";
const std::string CLR_YELLOW = "\033[33m";
const std::string CLR_RED = "\033[31m";
const std::string CLR_MAGENTA = "\033[35m";
const std::string CLR_WHITE = "\033[97m";

void printDivider(char ch = '=') {
    std::cout << std::string(76, ch) << '\n';
}

std::string healthBar(int health) {
    int clamped = health;
    if (clamped < 0) {
        clamped = 0;
    }
    if (clamped > 100) {
        clamped = 100;
    }

    int filled = clamped / 10;
    return std::string(static_cast<std::size_t>(filled), '#') +
           std::string(static_cast<std::size_t>(10 - filled), '.');
}

std::string statusColorForLabel(const std::string& label) {
    if (label == "CRITICAL") {
        return CLR_RED;
    }
    if (label == "LOW") {
        return CLR_YELLOW;
    }
    return CLR_GREEN;
}
} // namespace

Menu::Menu(Colony* colonyPtr, Simulation* simulationPtr, const std::string& dataDirectory)
    : colony(colonyPtr), simulation(simulationPtr), dataDirectory(dataDirectory), running(true) {}

int Menu::promptInt(const std::string& prompt, int minValue, int maxValue) const {
    while (true) {
        std::cout << CLR_CYAN << prompt << CLR_RESET;
        int value = 0;
        if (std::cin >> value) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            if (value >= minValue && value <= maxValue) {
                return value;
            }
        } else {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }

        std::cout << CLR_RED << "Invalid input. Enter a value from "
                  << minValue << " to " << maxValue << ".\n" << CLR_RESET;
    }
}

std::string Menu::promptLine(const std::string& prompt) const {
    std::cout << CLR_CYAN << prompt << CLR_RESET;
    std::string value;
    std::getline(std::cin >> std::ws, value);
    return value;
}

void Menu::waitForEnter() const {
    std::cout << CLR_BLUE << "\nPress ENTER to continue..." << CLR_RESET;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int Menu::getNextColonistId() const {
    int nextId = 100;
    for (const Colonist& colonist : colony->getColonists()) {
        if (colonist.getId() >= nextId) {
            nextId = colonist.getId() + 1;
        }
    }
    return nextId;
}

bool Menu::hasEnoughResources(const std::vector<ResourceCost>& costs) const {
    for (const ResourceCost& cost : costs) {
        const Resource* resource = colony->findResourceByName(cost.resourceName);
        if (resource == nullptr || resource->getQuantity() < cost.amount) {
            return false;
        }
    }
    return true;
}

void Menu::applyResourceCosts(const std::vector<ResourceCost>& costs) {
    for (const ResourceCost& cost : costs) {
        Resource* resource = colony->findResourceByName(cost.resourceName);
        if (resource != nullptr) {
            resource->consumeQuantity(cost.amount);
        }
    }
}

void Menu::printEventBox(const std::vector<std::string>& lines, const std::string& colorCode) const {
    std::size_t width = 30;
    for (const std::string& line : lines) {
        width = std::max(width, line.size() + 2);
    }

    std::cout << colorCode;
    std::cout << '+' << std::string(width, '-') << "+\n";
    for (const std::string& line : lines) {
        std::cout << "| " << std::left << std::setw(static_cast<int>(width - 1)) << line << "|\n";
    }
    std::cout << '+' << std::string(width, '-') << "+\n";
    std::cout << CLR_RESET;
}

void Menu::showTitle() const {
    std::cout << CLR_GREEN;
    std::cout << "\n";
    std::cout << "  ____      _                        ____                                          _\n";
    std::cout << " / ___|___ | | ___  _ __  _   _     / ___|___  _ __ ___  _ __ ___   __ _ _ __   __| |\n";
    std::cout << "| |   / _ \\| |/ _ \\| '_ \\| | | |   | |   / _ \\| '_ ` _ \\| '_ ` _ \\ / _` | '_ \\ / _` |\n";
    std::cout << "| |__| (_) | | (_) | | | | |_| |   | |__| (_) | | | | | | | | | | | (_| | | | | (_| |\n";
    std::cout << " \\____\\___/|_|\\___/|_| |_|\\__, |    \\____\\___/|_| |_| |_|_| |_| |_|\\__,_|_| |_|\\__,_|\n";
    std::cout << "                             |___/             Control                              \n";
    std::cout << CLR_RESET;
    printDivider('=');
}

void Menu::showMainMenu() const {
    std::cout << CLR_WHITE << "Day " << colony->getDayNumber() << " | Alive Colonists: "
              << colony->getAliveColonistCount() << CLR_RESET << '\n';
    std::cout << CLR_BLUE << "1) Overall Colony Status\n";
    std::cout << "2) Colonist Manager\n";
    std::cout << "3) Resource Manager\n";
    std::cout << "4) Structure Manager\n";
    std::cout << "5) Advance One Day\n";
    std::cout << "6) Disaster Console\n";
    std::cout << "7) Save Data\n";
    std::cout << "8) Reload Data\n";
    std::cout << "9) Exit\n" << CLR_RESET;
}

void Menu::showOverallStatus() const {
    printDivider('-');
    std::cout << CLR_MAGENTA << "COLONY STATUS REPORT" << CLR_RESET << '\n';
    printDivider('-');

    const std::vector<Colonist>& colonists = colony->getColonists();
    int totalHealth = 0;
    for (const Colonist& colonist : colonists) {
        totalHealth += colonist.getHealthLevel();
    }

    double averageHealth = colonists.empty() ? 0.0 : static_cast<double>(totalHealth) / colonists.size();

    std::cout << "Day Number: " << colony->getDayNumber() << '\n';
    std::cout << "Colonists Alive: " << colony->getAliveColonistCount() << " / " << colonists.size() << '\n';
    std::cout << "Average Health: " << std::fixed << std::setprecision(1) << averageHealth << "\n\n";

    displayResourcesTable(colony->getResources());
    std::cout << '\n';
    displayStructuresTable(colony->getStructures());

    const Resource* oxygen = colony->findResourceByName("Oxygen");
    if (oxygen != nullptr && oxygen->getQuantity() <= 25) {
        std::cout << CLR_RED << "\nWARNING: Oxygen is critically low. The air is becoming uncomfortable!\n" << CLR_RESET;
    }

    if (colony->isColonyFailed()) {
        std::cout << CLR_RED << "\nCOLONY FAILURE CONDITIONS MET. Immediate intervention required.\n" << CLR_RESET;
    } else if (colony->isColonyThriving()) {
        std::cout << CLR_GREEN << "\nColony is thriving. Command congratulates your leadership.\n" << CLR_RESET;
    }

    waitForEnter();
}

void Menu::displayColonistsTable(const std::vector<Colonist>& colonists) const {
    printDivider('-');
    std::cout << CLR_MAGENTA << "COLONISTS" << CLR_RESET << '\n';
    printDivider('-');
    std::cout << std::left
              << std::setw(6) << "ID"
              << std::setw(20) << "Name"
              << std::setw(16) << "Role"
              << std::setw(10) << "Health"
              << "Condition" << '\n';
    printDivider('.');

    for (const Colonist& colonist : colonists) {
        std::cout << std::left
                  << std::setw(6) << colonist.getId()
                  << std::setw(20) << colonist.getName()
                  << std::setw(16) << colonist.getRole()
                  << std::setw(10) << colonist.getHealthLevel()
                  << healthBar(colonist.getHealthLevel())
                  << '\n';
    }
}

void Menu::displayResourcesTable(const std::vector<Resource>& resources) const {
    printDivider('-');
    std::cout << CLR_MAGENTA << "RESOURCES" << CLR_RESET << '\n';
    printDivider('-');
    std::cout << std::left
              << std::setw(16) << "Resource"
              << std::setw(12) << "Quantity"
              << std::setw(18) << "Daily Rate"
              << "Status" << '\n';
    printDivider('.');

    for (const Resource& resource : resources) {
        std::string label = resource.getStatusLabel();
        std::cout << std::left
                  << std::setw(16) << resource.getResourceName()
                  << std::setw(12) << resource.getQuantity()
                  << std::setw(18) << resource.getDailyConsumptionRate()
                  << statusColorForLabel(label) << label << CLR_RESET
                  << '\n';
    }
}

void Menu::displayStructuresTable(const std::vector<Structure>& structures) const {
    printDivider('-');
    std::cout << CLR_MAGENTA << "STRUCTURES" << CLR_RESET << '\n';
    printDivider('-');
    std::cout << std::left
              << std::setw(22) << "Name"
              << std::setw(16) << "Type"
              << std::setw(12) << "Durability"
              << std::setw(8) << "Level"
              << "Operational" << '\n';
    printDivider('.');

    for (const Structure& structure : structures) {
        std::cout << std::left
                  << std::setw(22) << structure.getName()
                  << std::setw(16) << structure.getType()
                  << std::setw(12) << structure.getDurability()
                  << std::setw(8) << structure.getLevel()
                  << (structure.isOperational() ? "YES" : "NO")
                  << '\n';
    }
}

void Menu::colonistMenu() {
    bool inMenu = true;
    while (inMenu) {
        printDivider('=');
        std::cout << CLR_MAGENTA << "COLONIST MANAGER" << CLR_RESET << '\n';
        std::cout << "1) Display All Colonists\n";
        std::cout << "2) Add New Colonist\n";
        std::cout << "3) Remove Colonist\n";
        std::cout << "4) Heal One Colonist\n";
        std::cout << "5) Heal All Colonists\n";
        std::cout << "6) Sort by Highest Health\n";
        std::cout << "7) Sort by Role\n";
        std::cout << "8) Sort by Name\n";
        std::cout << "9) Back\n";

        int choice = promptInt("Select option: ", 1, 9);
        std::vector<Colonist>& colonists = colony->getColonists();

        switch (choice) {
        case 1:
            displayColonistsTable(colonists);
            waitForEnter();
            break;
        case 2: {
            std::string name = promptLine("Enter colonist name: ");
            std::string role = promptLine("Enter colonist role: ");
            int health = promptInt("Enter health (1-100): ", 1, 100);
            int id = getNextColonistId();
            colonists.push_back(Colonist(name, id, role, health));
            std::cout << CLR_GREEN << "Colonist added with ID " << id << ".\n" << CLR_RESET;
            waitForEnter();
            break;
        }
        case 3: {
            int id = promptInt("Enter ID to remove: ", 0, 1000000);
            if (colony->removeColonistById(id)) {
                std::cout << CLR_YELLOW << "Colonist removed.\n" << CLR_RESET;
            } else {
                std::cout << CLR_RED << "No colonist found with that ID.\n" << CLR_RESET;
            }
            waitForEnter();
            break;
        }
        case 4: {
            int id = promptInt("Enter ID to heal: ", 0, 1000000);
            Colonist* target = colony->findColonistById(id);
            if (target != nullptr) {
                int amount = promptInt("Heal amount (1-100): ", 1, 100);
                target->heal(amount);
                std::cout << CLR_GREEN << target->getName() << " healed to "
                          << target->getHealthLevel() << ".\n" << CLR_RESET;
            } else {
                std::cout << CLR_RED << "No colonist found with that ID.\n" << CLR_RESET;
            }
            waitForEnter();
            break;
        }
        case 5: {
            int amount = promptInt("Heal all by amount (1-50): ", 1, 50);
            for (Colonist& colonist : colonists) {
                Colonist* colonistPtr = &colonist;
                colonistPtr->heal(amount);
            }
            std::cout << CLR_GREEN << "All colonists healed.\n" << CLR_RESET;
            waitForEnter();
            break;
        }
        case 6:
            std::sort(colonists.begin(), colonists.end(),
                [](const Colonist& left, const Colonist& right) {
                    return left.getHealthLevel() > right.getHealthLevel();
                });
            std::cout << CLR_BLUE << "Sorted by highest health.\n" << CLR_RESET;
            waitForEnter();
            break;
        case 7:
            std::sort(colonists.begin(), colonists.end(),
                [](const Colonist& left, const Colonist& right) {
                    if (left.getRole() == right.getRole()) {
                        return left.getName() < right.getName();
                    }
                    return left.getRole() < right.getRole();
                });
            std::cout << CLR_BLUE << "Sorted by role.\n" << CLR_RESET;
            waitForEnter();
            break;
        case 8:
            std::sort(colonists.begin(), colonists.end(),
                [](const Colonist& left, const Colonist& right) {
                    return left.getName() < right.getName();
                });
            std::cout << CLR_BLUE << "Sorted by name.\n" << CLR_RESET;
            waitForEnter();
            break;
        case 9:
            inMenu = false;
            break;
        default:
            break;
        }
    }
}

void Menu::resourceMenu() {
    bool inMenu = true;
    while (inMenu) {
        printDivider('=');
        std::cout << CLR_MAGENTA << "RESOURCE MANAGER" << CLR_RESET << '\n';
        std::cout << "1) Display Resources\n";
        std::cout << "2) Update Quantity (Add/Subtract)\n";
        std::cout << "3) Simulate Daily Usage\n";
        std::cout << "4) Sort by Lowest Quantity\n";
        std::cout << "5) Check Resource Sufficiency\n";
        std::cout << "6) Back\n";

        int choice = promptInt("Select option: ", 1, 6);
        std::vector<Resource>& resources = colony->getResources();

        switch (choice) {
        case 1:
            displayResourcesTable(resources);
            waitForEnter();
            break;
        case 2: {
            if (resources.empty()) {
                std::cout << CLR_RED << "No resources available.\n" << CLR_RESET;
                waitForEnter();
                break;
            }

            for (std::size_t i = 0; i < resources.size(); ++i) {
                std::cout << (i + 1) << ") " << resources[i].getResourceName()
                          << " (" << resources[i].getQuantity() << ")\n";
            }

            int index = promptInt("Choose resource: ", 1, static_cast<int>(resources.size())) - 1;
            int mode = promptInt("1) Add  2) Subtract: ", 1, 2);
            int amount = promptInt("Amount: ", 1, 1000000);

            if (mode == 1) {
                resources[static_cast<std::size_t>(index)].addQuantity(amount);
                std::cout << CLR_GREEN << "Quantity updated.\n" << CLR_RESET;
            } else {
                int consumed = resources[static_cast<std::size_t>(index)].consumeQuantity(amount);
                std::cout << CLR_YELLOW << "Removed " << consumed << " units.\n" << CLR_RESET;
            }

            waitForEnter();
            break;
        }
        case 3: {
            std::vector<std::string> report = simulation->simulateResourceUsage();
            printEventBox(report, CLR_YELLOW);
            waitForEnter();
            break;
        }
        case 4:
            std::sort(resources.begin(), resources.end(),
                [](const Resource& left, const Resource& right) {
                    return left.getQuantity() < right.getQuantity();
                });
            std::cout << CLR_BLUE << "Resources sorted by lowest quantity.\n" << CLR_RESET;
            waitForEnter();
            break;
        case 5: {
            int needFood = promptInt("Required food: ", 0, 1000000);
            int needOxygen = promptInt("Required oxygen: ", 0, 1000000);
            int needEnergy = promptInt("Required energy: ", 0, 1000000);

            bool enough = true;
            Resource* food = colony->findResourceByName("Food");
            Resource* oxygen = colony->findResourceByName("Oxygen");
            Resource* energy = colony->findResourceByName("Energy");

            if (food == nullptr || oxygen == nullptr || energy == nullptr) {
                enough = false;
            } else {
                enough = food->isSufficient(needFood) &&
                         oxygen->isSufficient(needOxygen) &&
                         energy->isSufficient(needEnergy);
            }

            if (enough) {
                std::cout << CLR_GREEN << "Resources are sufficient for this action.\n" << CLR_RESET;
            } else {
                std::cout << CLR_RED << "Insufficient resources for this action.\n" << CLR_RESET;
            }
            waitForEnter();
            break;
        }
        case 6:
            inMenu = false;
            break;
        default:
            break;
        }
    }
}

void Menu::structureMenu() {
    bool inMenu = true;
    while (inMenu) {
        printDivider('=');
        std::cout << CLR_MAGENTA << "STRUCTURE MANAGER" << CLR_RESET << '\n';
        std::cout << "1) Display Structures\n";
        std::cout << "2) Add New Structure (Build Cost)\n";
        std::cout << "3) Repair Structure (Repair Cost)\n";
        std::cout << "4) Damage Structure\n";
        std::cout << "5) Sort by Durability\n";
        std::cout << "6) Back\n";

        int choice = promptInt("Select option: ", 1, 6);
        std::vector<Structure>& structures = colony->getStructures();

        switch (choice) {
        case 1:
            displayStructuresTable(structures);
            waitForEnter();
            break;
        case 2: {
            std::string name = promptLine("Structure name: ");
            std::cout << "Type options:\n";
            std::cout << "1) Habitat\n";
            std::cout << "2) Solar Farm\n";
            std::cout << "3) Greenhouse\n";
            std::cout << "4) Oxygen Plant\n";
            std::cout << "5) Water Extractor\n";

            int typeChoice = promptInt("Choose type: ", 1, 5);
            std::string type;
            if (typeChoice == 1) type = "Habitat";
            if (typeChoice == 2) type = "Solar Farm";
            if (typeChoice == 3) type = "Greenhouse";
            if (typeChoice == 4) type = "Oxygen Plant";
            if (typeChoice == 5) type = "Water Extractor";

            int level = promptInt("Level (1-5): ", 1, 5);
            Structure candidate(name, type, 100, level);
            std::vector<ResourceCost> costs = candidate.getBuildCosts();
            int requiredColonists = candidate.getBuildColonistRequirement();

            std::cout << "Build requirements:\n";
            std::cout << "  Colonists required: " << requiredColonists << '\n';
            for (const ResourceCost& cost : costs) {
                std::cout << "  " << cost.resourceName << ": " << cost.amount << '\n';
            }

            if (colony->getAliveColonistCount() < requiredColonists) {
                std::cout << CLR_RED << "Not enough healthy colonists available for construction.\n" << CLR_RESET;
                waitForEnter();
                break;
            }

            if (!hasEnoughResources(costs)) {
                std::cout << CLR_RED << "Not enough resources to build this structure.\n" << CLR_RESET;
                waitForEnter();
                break;
            }

            int confirm = promptInt("Proceed with construction? (1=Yes, 2=No): ", 1, 2);
            if (confirm == 1) {
                applyResourceCosts(costs);
                structures.push_back(candidate);
                std::cout << CLR_GREEN << "Structure built successfully.\n" << CLR_RESET;
            } else {
                std::cout << CLR_YELLOW << "Construction canceled.\n" << CLR_RESET;
            }
            waitForEnter();
            break;
        }
        case 3: {
            if (structures.empty()) {
                std::cout << CLR_RED << "No structures to repair.\n" << CLR_RESET;
                waitForEnter();
                break;
            }

            for (std::size_t i = 0; i < structures.size(); ++i) {
                std::cout << (i + 1) << ") " << structures[i].getName()
                          << " (Durability: " << structures[i].getDurability() << ")\n";
            }

            int index = promptInt("Choose structure: ", 1, static_cast<int>(structures.size())) - 1;
            Structure* target = &structures[static_cast<std::size_t>(index)];

            if (target->getDurability() >= 100) {
                std::cout << CLR_GREEN << "Structure is already at full durability.\n" << CLR_RESET;
                waitForEnter();
                break;
            }

            std::vector<ResourceCost> costs = target->getRepairCosts();
            int requiredColonists = target->getRepairColonistRequirement();

            std::cout << "Repair requirements:\n";
            std::cout << "  Colonists required: " << requiredColonists << '\n';
            for (const ResourceCost& cost : costs) {
                std::cout << "  " << cost.resourceName << ": " << cost.amount << '\n';
            }

            if (colony->getAliveColonistCount() < requiredColonists) {
                std::cout << CLR_RED << "Not enough colonists available for repairs.\n" << CLR_RESET;
                waitForEnter();
                break;
            }

            if (!hasEnoughResources(costs)) {
                std::cout << CLR_RED << "Not enough resources to perform repairs.\n" << CLR_RESET;
                waitForEnter();
                break;
            }

            applyResourceCosts(costs);
            target->repair(100);
            std::cout << CLR_GREEN << "Repairs complete. " << target->getName()
                      << " restored to full durability.\n" << CLR_RESET;
            waitForEnter();
            break;
        }
        case 4: {
            if (structures.empty()) {
                std::cout << CLR_RED << "No structures available.\n" << CLR_RESET;
                waitForEnter();
                break;
            }

            for (std::size_t i = 0; i < structures.size(); ++i) {
                std::cout << (i + 1) << ") " << structures[i].getName()
                          << " (Durability: " << structures[i].getDurability() << ")\n";
            }

            int index = promptInt("Choose structure to damage: ", 1, static_cast<int>(structures.size())) - 1;
            int amount = promptInt("Damage amount (1-100): ", 1, 100);
            structures[static_cast<std::size_t>(index)].damage(amount);

            std::cout << CLR_YELLOW << "Damage applied. New durability: "
                      << structures[static_cast<std::size_t>(index)].getDurability() << "\n" << CLR_RESET;
            if (!structures[static_cast<std::size_t>(index)].isOperational()) {
                std::cout << CLR_RED << "Structure has collapsed and is no longer operational.\n" << CLR_RESET;
            }
            waitForEnter();
            break;
        }
        case 5:
            std::sort(structures.begin(), structures.end(),
                [](const Structure& left, const Structure& right) {
                    return left.getDurability() > right.getDurability();
                });
            std::cout << CLR_BLUE << "Structures sorted by durability (highest first).\n" << CLR_RESET;
            waitForEnter();
            break;
        case 6:
            inMenu = false;
            break;
        default:
            break;
        }
    }
}

void Menu::disasterMenu() {
    bool inMenu = true;
    while (inMenu) {
        printDivider('=');
        std::cout << CLR_MAGENTA << "DISASTER CONSOLE" << CLR_RESET << '\n';
        std::cout << "1) Trigger Random Event\n";
        std::cout << "2) Trigger Meteor Strike\n";
        std::cout << "3) Trigger Illness Outbreak\n";
        std::cout << "4) Trigger Sandstorm\n";
        std::cout << "5) Trigger Power Surge\n";
        std::cout << "6) Trigger Alien Sighting\n";
        std::cout << "7) Back\n";

        int choice = promptInt("Select option: ", 1, 7);

        if (choice == 7) {
            inMenu = false;
            continue;
        }

        std::vector<std::string> report;
        if (choice == 1) {
            report = simulation->triggerRandomEvent();
        } else {
            report = simulation->triggerSpecificEvent(choice - 2);
        }

        printEventBox(report, CLR_RED);
        waitForEnter();
    }
}

void Menu::run() {
    while (running) {
        showTitle();
        showMainMenu();

        int choice = promptInt("\nChoose command: ", 1, 9);

        switch (choice) {
        case 1:
            showOverallStatus();
            break;
        case 2:
            colonistMenu();
            break;
        case 3:
            resourceMenu();
            break;
        case 4:
            structureMenu();
            break;
        case 5: {
            std::vector<std::string> report = simulation->advanceDay();
            printEventBox(report, CLR_GREEN);
            waitForEnter();
            break;
        }
        case 6:
            disasterMenu();
            break;
        case 7: {
            if (FileManager::saveAll(dataDirectory, *colony)) {
                std::cout << CLR_GREEN << "Data saved successfully.\n" << CLR_RESET;
            } else {
                std::cout << CLR_RED << "Save failed. Check file permissions.\n" << CLR_RESET;
            }
            waitForEnter();
            break;
        }
        case 8: {
            bool fullyLoaded = FileManager::loadAll(dataDirectory, *colony);
            if (fullyLoaded) {
                std::cout << CLR_GREEN << "Data loaded successfully.\n" << CLR_RESET;
            } else {
                std::cout << CLR_YELLOW
                          << "Data was partially missing. Defaults were used for missing modules.\n"
                          << CLR_RESET;
            }
            waitForEnter();
            break;
        }
        case 9:
            FileManager::saveAll(dataDirectory, *colony);
            std::cout << CLR_GREEN << "Auto-save complete. Shutting down colony console.\n" << CLR_RESET;
            running = false;
            break;
        default:
            break;
        }

        if (colony->isColonyFailed()) {
            std::vector<std::string> collapseMessage;
            collapseMessage.push_back("COLONY FAILURE DETECTED");
            collapseMessage.push_back("Life support has collapsed or all colonists are lost.");
            collapseMessage.push_back("You can still continue to inspect/save, or reload a previous state.");
            printEventBox(collapseMessage, CLR_RED);
        }
    }
}
