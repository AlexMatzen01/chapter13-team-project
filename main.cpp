#include "Colony.h"
#include "FileManager.h"
#include "Menu.h"
#include "Simulation.h"

#include <exception>
#include <iostream>

int main() {
    try {
        const std::string dataDirectory = "data";

        Colony colony;
        FileManager::loadAll(dataDirectory, colony);

        Simulation simulation(&colony);
        Menu menu(&colony, &simulation, dataDirectory);
        menu.run();

        if (!FileManager::saveAll(dataDirectory, colony)) {
            std::cerr << "Warning: Failed to save colony data on shutdown.\n";
            return 1;
        }

        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "Fatal error: " << ex.what() << '\n';
        return 1;
    } catch (...) {
        std::cerr << "Fatal error: Unknown exception occurred.\n";
        return 1;
    }
}
