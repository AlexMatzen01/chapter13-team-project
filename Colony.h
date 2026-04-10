#ifndef COLONY_H
#define COLONY_H

#include "Colonist.h"
#include "Resource.h"
#include "Structure.h"

#include <string>
#include <vector>

class Colony {
private:
    std::vector<Colonist> colonists;
    std::vector<Resource> resources;
    std::vector<Structure> structures;
    int dayNumber;

public:
    Colony();

    std::vector<Colonist>& getColonists();
    const std::vector<Colonist>& getColonists() const;

    std::vector<Resource>& getResources();
    const std::vector<Resource>& getResources() const;

    std::vector<Structure>& getStructures();
    const std::vector<Structure>& getStructures() const;

    int getDayNumber() const;
    void setDayNumber(int newDayNumber);
    void incrementDay();

    Resource* findResourceByName(const std::string& name);
    const Resource* findResourceByName(const std::string& name) const;

    Colonist* findColonistById(int id);
    const Colonist* findColonistById(int id) const;

    Structure* findStructureByName(const std::string& name);
    const Structure* findStructureByName(const std::string& name) const;

    bool removeColonistById(int id);
    int getAliveColonistCount() const;
    int removeDeadColonists(std::vector<std::string>* removedNames = nullptr);

    bool isColonyFailed() const;
    bool isColonyThriving() const;
};

#endif
