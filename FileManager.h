#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include "Colony.h"

#include <string>

class FileManager {
public:
    static bool loadAll(const std::string& dataDir, Colony& colony);
    static bool saveAll(const std::string& dataDir, const Colony& colony);
    static void initializeDefaults(Colony& colony);
    static void ensureDataDirectory(const std::string& dataDir);

private:
    static bool loadColonists(const std::string& path, Colony& colony);
    static bool loadResources(const std::string& path, Colony& colony);
    static bool loadStructures(const std::string& path, Colony& colony);

    static bool saveColonists(const std::string& path, const Colony& colony);
    static bool saveResources(const std::string& path, const Colony& colony);
    static bool saveStructures(const std::string& path, const Colony& colony);
};

#endif
