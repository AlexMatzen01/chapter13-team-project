#ifndef RESOURCE_H
#define RESOURCE_H

#include <string>

class Resource {
private:
    std::string resourceName;
    int quantity;
    double dailyConsumptionRate;

public:
    Resource();
    Resource(const std::string& resourceName, int quantity, double dailyConsumptionRate);

    const std::string& getResourceName() const;
    int getQuantity() const;
    double getDailyConsumptionRate() const;

    void setResourceName(const std::string& newName);
    void setQuantity(int newQuantity);
    void setDailyConsumptionRate(double newRate);

    void addQuantity(int amount);
    int consumeQuantity(int amount);
    bool isSufficient(int requiredAmount) const;

    std::string getStatusLabel() const;

    std::string toDataString() const;
    static bool fromDataString(const std::string& line, Resource& outResource);

    static std::string normalizeName(const std::string& rawName);
};

#endif
