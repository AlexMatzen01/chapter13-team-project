#include "Colony.h"

#include <algorithm>
#include <cctype>

namespace {
std::string toLowerCopy(const std::string& text) {
    std::string result = text;
    std::transform(result.begin(), result.end(), result.begin(),
        [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });
    return result;
}
} // namespace

Colony::Colony() : dayNumber(1) {}

std::vector<Colonist>& Colony::getColonists() {
    return colonists;
}

const std::vector<Colonist>& Colony::getColonists() const {
    return colonists;
}

std::vector<Resource>& Colony::getResources() {
    return resources;
}

const std::vector<Resource>& Colony::getResources() const {
    return resources;
}

std::vector<Structure>& Colony::getStructures() {
    return structures;
}

const std::vector<Structure>& Colony::getStructures() const {
    return structures;
}

int Colony::getDayNumber() const {
    return dayNumber;
}

void Colony::setDayNumber(int newDayNumber) {
    dayNumber = (newDayNumber < 1) ? 1 : newDayNumber;
}

void Colony::incrementDay() {
    ++dayNumber;
}

Resource* Colony::findResourceByName(const std::string& name) {
    std::string key = toLowerCopy(name);
    for (Resource& resource : resources) {
        if (toLowerCopy(resource.getResourceName()) == key) {
            return &resource;
        }
    }
    return nullptr;
}

const Resource* Colony::findResourceByName(const std::string& name) const {
    std::string key = toLowerCopy(name);
    for (const Resource& resource : resources) {
        if (toLowerCopy(resource.getResourceName()) == key) {
            return &resource;
        }
    }
    return nullptr;
}

Colonist* Colony::findColonistById(int id) {
    for (Colonist& colonist : colonists) {
        if (colonist.getId() == id) {
            return &colonist;
        }
    }
    return nullptr;
}

const Colonist* Colony::findColonistById(int id) const {
    for (const Colonist& colonist : colonists) {
        if (colonist.getId() == id) {
            return &colonist;
        }
    }
    return nullptr;
}

Structure* Colony::findStructureByName(const std::string& name) {
    std::string key = toLowerCopy(name);
    for (Structure& structure : structures) {
        if (toLowerCopy(structure.getName()) == key) {
            return &structure;
        }
    }
    return nullptr;
}

const Structure* Colony::findStructureByName(const std::string& name) const {
    std::string key = toLowerCopy(name);
    for (const Structure& structure : structures) {
        if (toLowerCopy(structure.getName()) == key) {
            return &structure;
        }
    }
    return nullptr;
}

bool Colony::removeColonistById(int id) {
    std::size_t oldSize = colonists.size();
    colonists.erase(
        std::remove_if(colonists.begin(), colonists.end(),
            [id](const Colonist& colonist) { return colonist.getId() == id; }),
        colonists.end());
    return colonists.size() != oldSize;
}

int Colony::getAliveColonistCount() const {
    int alive = 0;
    for (const Colonist& colonist : colonists) {
        if (colonist.isAlive()) {
            ++alive;
        }
    }
    return alive;
}

int Colony::removeDeadColonists(std::vector<std::string>* removedNames) {
    int removedCount = 0;
    auto iter = colonists.begin();
    while (iter != colonists.end()) {
        if (!iter->isAlive()) {
            if (removedNames != nullptr) {
                removedNames->push_back(iter->getName());
            }
            iter = colonists.erase(iter);
            ++removedCount;
        } else {
            ++iter;
        }
    }
    return removedCount;
}

bool Colony::isColonyFailed() const {
    if (colonists.empty() || getAliveColonistCount() == 0) {
        return true;
    }

    const Resource* oxygen = findResourceByName("Oxygen");
    if (oxygen != nullptr && oxygen->getQuantity() <= 0) {
        return true;
    }

    return false;
}

bool Colony::isColonyThriving() const {
    if (getAliveColonistCount() < 5) {
        return false;
    }

    const Resource* food = findResourceByName("Food");
    const Resource* oxygen = findResourceByName("Oxygen");
    const Resource* energy = findResourceByName("Energy");

    if (food == nullptr || oxygen == nullptr || energy == nullptr) {
        return false;
    }

    return food->getQuantity() > 150 && oxygen->getQuantity() > 180 && energy->getQuantity() > 150;
}
