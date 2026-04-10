#include "Colonist.h"

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
} // namespace

Colonist::Colonist() : name("Unnamed"), id(0), role("Worker"), healthLevel(100) {}

Colonist::Colonist(const std::string& name, int id, const std::string& role, int healthLevel)
    : name(trim(name)), id(id), role(formatRole(role)), healthLevel(healthLevel) {
    if (this->name.empty()) {
        this->name = "Unnamed";
    }
    if (this->id < 0) {
        this->id = 0;
    }
    if (this->healthLevel < 0) {
        this->healthLevel = 0;
    }
    if (this->healthLevel > 100) {
        this->healthLevel = 100;
    }
}

const std::string& Colonist::getName() const {
    return name;
}

int Colonist::getId() const {
    return id;
}

const std::string& Colonist::getRole() const {
    return role;
}

int Colonist::getHealthLevel() const {
    return healthLevel;
}

void Colonist::setName(const std::string& newName) {
    std::string cleaned = trim(newName);
    if (!cleaned.empty()) {
        name = cleaned;
    }
}

void Colonist::setId(int newId) {
    if (newId >= 0) {
        id = newId;
    }
}

void Colonist::setRole(const std::string& newRole) {
    role = formatRole(newRole);
}

void Colonist::setHealthLevel(int newHealthLevel) {
    if (newHealthLevel < 0) {
        healthLevel = 0;
    } else if (newHealthLevel > 100) {
        healthLevel = 100;
    } else {
        healthLevel = newHealthLevel;
    }
}

void Colonist::adjustHealth(int delta) {
    setHealthLevel(healthLevel + delta);
}

void Colonist::heal(int amount) {
    if (amount > 0) {
        adjustHealth(amount);
    }
}

void Colonist::damage(int amount) {
    if (amount > 0) {
        adjustHealth(-amount);
    }
}

bool Colonist::isAlive() const {
    return healthLevel > 0;
}

std::string Colonist::toDataString() const {
    std::ostringstream out;
    out << sanitizeField(name) << '|' << id << '|' << sanitizeField(role) << '|' << healthLevel;
    return out.str();
}

bool Colonist::fromDataString(const std::string& line, Colonist& outColonist) {
    if (trim(line).empty() || line[0] == '#') {
        return false;
    }

    std::stringstream stream(line);
    std::string nameField;
    std::string idField;
    std::string roleField;
    std::string healthField;

    if (!std::getline(stream, nameField, '|') ||
        !std::getline(stream, idField, '|') ||
        !std::getline(stream, roleField, '|') ||
        !std::getline(stream, healthField)) {
        return false;
    }

    try {
        int parsedId = std::stoi(trim(idField));
        int parsedHealth = std::stoi(trim(healthField));
        outColonist = Colonist(trim(nameField), parsedId, trim(roleField), parsedHealth);
        return true;
    } catch (...) {
        return false;
    }
}

std::string Colonist::formatRole(const std::string& rawRole) {
    std::string cleaned = trim(rawRole);
    if (cleaned.empty()) {
        return "Worker";
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
