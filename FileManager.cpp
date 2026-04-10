#include "FileManager.h"

#include <filesystem>
#include <fstream>
#include <sstream>

namespace {
const char* COLONISTS_FILE = "colonists.txt";
const char* RESOURCES_FILE = "resources.txt";
const char* STRUCTURES_FILE = "structures.txt";

std::string buildPath(const std::string& dataDir, const std::string& fileName) {
    std::filesystem::path fullPath = std::filesystem::path(dataDir) / fileName;
    return fullPath.string();
}

std::string trim(const std::string& text) {
    std::size_t first = text.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) {
        return "";
    }
    std::size_t last = text.find_last_not_of(" \t\r\n");
    return text.substr(first, last - first + 1);
}
} // namespace

bool FileManager::loadAll(const std::string& dataDir, Colony& colony) {
    ensureDataDirectory(dataDir);

    bool colonistsLoaded = loadColonists(buildPath(dataDir, COLONISTS_FILE), colony);
    bool resourcesLoaded = loadResources(buildPath(dataDir, RESOURCES_FILE), colony);
    bool structuresLoaded = loadStructures(buildPath(dataDir, STRUCTURES_FILE), colony);

    if (!colonistsLoaded || colony.getColonists().empty()) {
        colony.getColonists().clear();
        colony.getColonists().push_back(Colonist("Ava Stone", 101, "Engineer", 92));
        colony.getColonists().push_back(Colonist("Noah Vega", 102, "Medic", 88));
        colony.getColonists().push_back(Colonist("Mira Sol", 103, "Farmer", 95));
        colony.getColonists().push_back(Colonist("Rex Arden", 104, "Technician", 84));
        colony.getColonists().push_back(Colonist("Lena Frost", 105, "Scientist", 90));
    }

    if (!resourcesLoaded || colony.getResources().empty()) {
        colony.getResources().clear();
        colony.getResources().push_back(Resource("Food", 180, 1.2));
        colony.getResources().push_back(Resource("Oxygen", 260, 2.0));
        colony.getResources().push_back(Resource("Energy", 220, 1.5));
    }

    if (!structuresLoaded || colony.getStructures().empty()) {
        colony.getStructures().clear();
        colony.getStructures().push_back(Structure("Aurora Habitat", "Habitat", 92, 2));
        colony.getStructures().push_back(Structure("Helios Array", "Solar Farm", 87, 2));
        colony.getStructures().push_back(Structure("Verdant Dome", "Greenhouse", 95, 2));
        colony.getStructures().push_back(Structure("O2 Nexus", "Oxygen Plant", 88, 1));
    }

    if (colony.getDayNumber() < 1) {
        colony.setDayNumber(1);
    }

    saveAll(dataDir, colony);

    return colonistsLoaded && resourcesLoaded && structuresLoaded;
}

bool FileManager::saveAll(const std::string& dataDir, const Colony& colony) {
    ensureDataDirectory(dataDir);

    bool colonistsSaved = saveColonists(buildPath(dataDir, COLONISTS_FILE), colony);
    bool resourcesSaved = saveResources(buildPath(dataDir, RESOURCES_FILE), colony);
    bool structuresSaved = saveStructures(buildPath(dataDir, STRUCTURES_FILE), colony);

    return colonistsSaved && resourcesSaved && structuresSaved;
}

void FileManager::initializeDefaults(Colony& colony) {
    colony.getColonists().clear();
    colony.getResources().clear();
    colony.getStructures().clear();

    colony.setDayNumber(1);

    colony.getColonists().push_back(Colonist("Ava Stone", 101, "Engineer", 92));
    colony.getColonists().push_back(Colonist("Noah Vega", 102, "Medic", 88));
    colony.getColonists().push_back(Colonist("Mira Sol", 103, "Farmer", 95));
    colony.getColonists().push_back(Colonist("Rex Arden", 104, "Technician", 84));
    colony.getColonists().push_back(Colonist("Lena Frost", 105, "Scientist", 90));

    colony.getResources().push_back(Resource("Food", 180, 1.2));
    colony.getResources().push_back(Resource("Oxygen", 260, 2.0));
    colony.getResources().push_back(Resource("Energy", 220, 1.5));

    colony.getStructures().push_back(Structure("Aurora Habitat", "Habitat", 92, 2));
    colony.getStructures().push_back(Structure("Helios Array", "Solar Farm", 87, 2));
    colony.getStructures().push_back(Structure("Verdant Dome", "Greenhouse", 95, 2));
    colony.getStructures().push_back(Structure("O2 Nexus", "Oxygen Plant", 88, 1));
}

void FileManager::ensureDataDirectory(const std::string& dataDir) {
    std::error_code error;
    std::filesystem::create_directories(dataDir, error);
}

bool FileManager::loadColonists(const std::string& path, Colony& colony) {
    std::ifstream input(path);
    if (!input.is_open()) {
        return false;
    }

    std::vector<Colonist> loaded;
    std::string line;
    while (std::getline(input, line)) {
        std::string cleaned = trim(line);
        if (cleaned.empty()) {
            continue;
        }
        if (cleaned.rfind("#DAY=", 0) == 0) {
            try {
                colony.setDayNumber(std::stoi(cleaned.substr(5)));
            } catch (...) {
                colony.setDayNumber(1);
            }
            continue;
        }

        Colonist colonist;
        if (Colonist::fromDataString(cleaned, colonist)) {
            loaded.push_back(colonist);
        }
    }

    if (loaded.empty()) {
        return false;
    }

    colony.getColonists() = loaded;
    return true;
}

bool FileManager::loadResources(const std::string& path, Colony& colony) {
    std::ifstream input(path);
    if (!input.is_open()) {
        return false;
    }

    std::vector<Resource> loaded;
    std::string line;
    while (std::getline(input, line)) {
        Resource resource;
        if (Resource::fromDataString(line, resource)) {
            loaded.push_back(resource);
        }
    }

    if (loaded.empty()) {
        return false;
    }

    colony.getResources() = loaded;
    return true;
}

bool FileManager::loadStructures(const std::string& path, Colony& colony) {
    std::ifstream input(path);
    if (!input.is_open()) {
        return false;
    }

    std::vector<Structure> loaded;
    std::string line;
    while (std::getline(input, line)) {
        Structure structure;
        if (Structure::fromDataString(line, structure)) {
            loaded.push_back(structure);
        }
    }

    if (loaded.empty()) {
        return false;
    }

    colony.getStructures() = loaded;
    return true;
}

bool FileManager::saveColonists(const std::string& path, const Colony& colony) {
    std::ofstream output(path, std::ios::trunc);
    if (!output.is_open()) {
        return false;
    }

    output << "#DAY=" << colony.getDayNumber() << '\n';
    for (const Colonist& colonist : colony.getColonists()) {
        output << colonist.toDataString() << '\n';
    }

    return true;
}

bool FileManager::saveResources(const std::string& path, const Colony& colony) {
    std::ofstream output(path, std::ios::trunc);
    if (!output.is_open()) {
        return false;
    }

    for (const Resource& resource : colony.getResources()) {
        output << resource.toDataString() << '\n';
    }

    return true;
}

bool FileManager::saveStructures(const std::string& path, const Colony& colony) {
    std::ofstream output(path, std::ios::trunc);
    if (!output.is_open()) {
        return false;
    }

    for (const Structure& structure : colony.getStructures()) {
        output << structure.toDataString() << '\n';
    }

    return true;
}
