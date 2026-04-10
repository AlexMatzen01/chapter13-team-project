#include "Structure.h"

#include <algorithm>
#include <cctype>
#include <sstream>

namespace {
std::string trim(const std::string& text) {
    std::size_t first = text.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) {
        return "";
    }
    std::size_t last = text.find_last_not_of(" \t\r\n");
    return text.substr(first, last - first + 1);
}

std::string sanitizeField(const std::string& value) {
    std::string clean = value;
    std::replace(clean.begin(), clean.end(), '|', '/');
    return clean;
}

void addCost(std::vector<ResourceCost>& costs, const std::string& name, int amount) {
    if (amount > 0) {
        costs.push_back(ResourceCost{name, amount});
    }
}
} // namespace

Structure::Structure() : name("Unnamed Structure"), type("Habitat"), durability(100), level(1) {}

Structure::Structure(const std::string& name, const std::string& type, int durability, int level)
    : name(trim(name)), type(normalizeType(type)), durability(durability), level(level) {
    if (this->name.empty()) {
        this->name = "Unnamed Structure";
    }
    setDurability(this->durability);
    setLevel(this->level);
}

const std::string& Structure::getName() const {
    return name;
}

const std::string& Structure::getType() const {
    return type;
}

int Structure::getDurability() const {
    return durability;
}

int Structure::getLevel() const {
    return level;
}

void Structure::setName(const std::string& newName) {
    std::string cleaned = trim(newName);
    if (!cleaned.empty()) {
        name = cleaned;
    }
}

void Structure::setType(const std::string& newType) {
    type = normalizeType(newType);
}

void Structure::setDurability(int newDurability) {
    if (newDurability < 0) {
        durability = 0;
    } else if (newDurability > 100) {
        durability = 100;
    } else {
        durability = newDurability;
    }
}

void Structure::setLevel(int newLevel) {
    if (newLevel < 1) {
        level = 1;
    } else if (newLevel > 5) {
        level = 5;
    } else {
        level = newLevel;
    }
}

void Structure::damage(int amount) {
    if (amount > 0) {
        setDurability(durability - amount);
    }
}

void Structure::repair(int amount) {
    if (amount > 0) {
        setDurability(durability + amount);
    }
}

bool Structure::isOperational() const {
    return durability > 0;
}

std::vector<ResourceCost> Structure::getDailyProduction() const {
    std::vector<ResourceCost> production;
    if (!isOperational()) {
        return production;
    }

    double durabilityMultiplier = static_cast<double>(durability) / 100.0;
    int scaledLevel = level;

    if (type == "Solar Farm") {
        addCost(production, "Energy", static_cast<int>((20 * scaledLevel) * durabilityMultiplier));
    } else if (type == "Greenhouse") {
        addCost(production, "Food", static_cast<int>((16 * scaledLevel) * durabilityMultiplier));
    } else if (type == "Oxygen Plant") {
        addCost(production, "Oxygen", static_cast<int>((18 * scaledLevel) * durabilityMultiplier));
    } else if (type == "Habitat") {
        addCost(production, "Oxygen", static_cast<int>((6 * scaledLevel) * durabilityMultiplier));
    } else if (type == "Water Extractor") {
        addCost(production, "Food", static_cast<int>((8 * scaledLevel) * durabilityMultiplier));
        addCost(production, "Oxygen", static_cast<int>((5 * scaledLevel) * durabilityMultiplier));
    } else {
        addCost(production, "Energy", static_cast<int>((8 * scaledLevel) * durabilityMultiplier));
    }

    return production;
}

std::vector<ResourceCost> Structure::getBuildCosts() const {
    std::vector<ResourceCost> costs;

    if (type == "Solar Farm") {
        addCost(costs, "Food", 12 * level);
        addCost(costs, "Oxygen", 8 * level);
        addCost(costs, "Energy", 24 * level);
    } else if (type == "Greenhouse") {
        addCost(costs, "Food", 8 * level);
        addCost(costs, "Oxygen", 10 * level);
        addCost(costs, "Energy", 18 * level);
    } else if (type == "Oxygen Plant") {
        addCost(costs, "Food", 10 * level);
        addCost(costs, "Oxygen", 6 * level);
        addCost(costs, "Energy", 20 * level);
    } else if (type == "Habitat") {
        addCost(costs, "Food", 20 * level);
        addCost(costs, "Oxygen", 22 * level);
        addCost(costs, "Energy", 26 * level);
    } else if (type == "Water Extractor") {
        addCost(costs, "Food", 14 * level);
        addCost(costs, "Oxygen", 12 * level);
        addCost(costs, "Energy", 18 * level);
    } else {
        addCost(costs, "Food", 10 * level);
        addCost(costs, "Oxygen", 10 * level);
        addCost(costs, "Energy", 14 * level);
    }

    return costs;
}

std::vector<ResourceCost> Structure::getRepairCosts() const {
    std::vector<ResourceCost> costs;
    if (durability >= 100) {
        return costs;
    }

    int missingDurability = 100 - durability;
    int severity = std::max(1, missingDurability / 10);

    addCost(costs, "Food", severity);
    addCost(costs, "Oxygen", severity);
    addCost(costs, "Energy", severity * 2);

    return costs;
}

int Structure::getBuildColonistRequirement() const {
    if (type == "Habitat") {
        return 3 + (level / 2);
    }
    if (type == "Solar Farm" || type == "Oxygen Plant") {
        return 2 + (level / 3);
    }
    return 2;
}

int Structure::getRepairColonistRequirement() const {
    if (durability >= 100) {
        return 0;
    }

    int missingDurability = 100 - durability;
    return 1 + (missingDurability / 30);
}

std::string Structure::toDataString() const {
    std::ostringstream out;
    out << sanitizeField(name) << '|' << sanitizeField(type) << '|' << durability << '|' << level;
    return out.str();
}

bool Structure::fromDataString(const std::string& line, Structure& outStructure) {
    std::string cleanedLine = trim(line);
    if (cleanedLine.empty() || cleanedLine[0] == '#') {
        return false;
    }

    std::stringstream stream(cleanedLine);
    std::string nameField;
    std::string typeField;
    std::string durabilityField;
    std::string levelField;

    if (!std::getline(stream, nameField, '|') ||
        !std::getline(stream, typeField, '|') ||
        !std::getline(stream, durabilityField, '|') ||
        !std::getline(stream, levelField)) {
        return false;
    }

    try {
        int parsedDurability = std::stoi(trim(durabilityField));
        int parsedLevel = std::stoi(trim(levelField));
        outStructure = Structure(trim(nameField), trim(typeField), parsedDurability, parsedLevel);
        return true;
    } catch (...) {
        return false;
    }
}

std::string Structure::normalizeType(const std::string& rawType) {
    std::string cleaned = trim(rawType);
    if (cleaned.empty()) {
        return "Habitat";
    }

    std::transform(cleaned.begin(), cleaned.end(), cleaned.begin(),
        [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });

    bool capitalize = true;
    for (char& ch : cleaned) {
        if (std::isspace(static_cast<unsigned char>(ch)) || ch == '-') {
            capitalize = true;
            continue;
        }
        if (capitalize) {
            ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
            capitalize = false;
        }
    }

    return cleaned;
}
