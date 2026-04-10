#ifndef COLONIST_H
#define COLONIST_H

#include <string>

class Colonist {
private:
    std::string name;
    int id;
    std::string role;
    int healthLevel;

public:
    Colonist();
    Colonist(const std::string& name, int id, const std::string& role, int healthLevel);

    const std::string& getName() const;
    int getId() const;
    const std::string& getRole() const;
    int getHealthLevel() const;

    void setName(const std::string& newName);
    void setId(int newId);
    void setRole(const std::string& newRole);
    void setHealthLevel(int newHealthLevel);

    void adjustHealth(int delta);
    void heal(int amount = 20);
    void damage(int amount = 15);
    bool isAlive() const;

    std::string toDataString() const;
    static bool fromDataString(const std::string& line, Colonist& outColonist);

    static std::string formatRole(const std::string& rawRole);
};

#endif
