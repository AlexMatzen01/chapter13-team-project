#include "Resource.h"

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

Resource::Resource() : resourceName("Food"), quantity(0), dailyConsumptionRate(1.0) {}

Resource::Resource(const std::string& resourceName, int quantity, double dailyConsumptionRate)
    : resourceName(normalizeName(resourceName)), quantity(quantity), dailyConsumptionRate(dailyConsumptionRate) {
    if (this->quantity < 0) {
        this->quantity = 0;
    }
    if (this->dailyConsumptionRate < 0.0) {
        this->dailyConsumptionRate = 0.0;
    }
}

const std::string& Resource::getResourceName() const {
    return resourceName;
}

int Resource::getQuantity() const {
    return quantity;
}

double Resource::getDailyConsumptionRate() const {
    return dailyConsumptionRate;
}

void Resource::setResourceName(const std::string& newName) {
    resourceName = normalizeName(newName);
}

void Resource::setQuantity(int newQuantity) {
    quantity = (newQuantity < 0) ? 0 : newQuantity;
}

void Resource::setDailyConsumptionRate(double newRate) {
    dailyConsumptionRate = (newRate < 0.0) ? 0.0 : newRate;
}

void Resource::addQuantity(int amount) {
    if (amount > 0) {
        quantity += amount;
    }
}

int Resource::consumeQuantity(int amount) {
    if (amount <= 0) {
        return 0;
    }
    int consumed = (amount > quantity) ? quantity : amount;
    quantity -= consumed;
    return consumed;
}

bool Resource::isSufficient(int requiredAmount) const {
    return requiredAmount <= quantity;
}

std::string Resource::getStatusLabel() const {
    if (quantity <= 25) {
        return "CRITICAL";
    }
    if (quantity <= 80) {
        return "LOW";
    }
    return "OK";
}

std::string Resource::toDataString() const {
    std::ostringstream out;
    out << sanitizeField(resourceName) << '|' << quantity << '|' << dailyConsumptionRate;
    return out.str();
}

bool Resource::fromDataString(const std::string& line, Resource& outResource) {
    std::string cleanedLine = trim(line);
    if (cleanedLine.empty() || cleanedLine[0] == '#') {
        return false;
    }

    std::stringstream stream(cleanedLine);
    std::string nameField;
    std::string quantityField;
    std::string rateField;

    if (!std::getline(stream, nameField, '|') ||
        !std::getline(stream, quantityField, '|') ||
        !std::getline(stream, rateField)) {
        return false;
    }

    try {
        int parsedQuantity = std::stoi(trim(quantityField));
        double parsedRate = std::stod(trim(rateField));
        outResource = Resource(trim(nameField), parsedQuantity, parsedRate);
        return true;
    } catch (...) {
        return false;
    }
}

std::string Resource::normalizeName(const std::string& rawName) {
    std::string cleaned = trim(rawName);
    if (cleaned.empty()) {
        return "Resource";
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
