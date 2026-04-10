#ifndef STRUCTURE_H
#define STRUCTURE_H

#include <string>
#include <vector>

struct ResourceCost {
    std::string resourceName;
    int amount;
};

class Structure {
private:
    std::string name;
    std::string type;
    int durability;
    int level;

public:
    Structure();
    Structure(const std::string& name, const std::string& type, int durability, int level = 1);

    const std::string& getName() const;
    const std::string& getType() const;
    int getDurability() const;
    int getLevel() const;

    void setName(const std::string& newName);
    void setType(const std::string& newType);
    void setDurability(int newDurability);
    void setLevel(int newLevel);

    void damage(int amount);
    void repair(int amount);
    bool isOperational() const;

    std::vector<ResourceCost> getDailyProduction() const;
    std::vector<ResourceCost> getBuildCosts() const;
    std::vector<ResourceCost> getRepairCosts() const;

    int getBuildColonistRequirement() const;
    int getRepairColonistRequirement() const;

    std::string toDataString() const;
    static bool fromDataString(const std::string& line, Structure& outStructure);

    static std::string normalizeType(const std::string& rawType);
};

#endif
