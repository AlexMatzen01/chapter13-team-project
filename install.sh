#!/bin/bash

# Exit immediately if a command exits with a non-zero status
set -e

echo "Setting up Colony Simulation Project environment..."

# 1. Check if the g++ compiler is installed (assuming Ubuntu/Debian in WSL)
if ! command -v g++ &> /dev/null
then
    echo "g++ compiler not found. Attempting to install build-essential..."
    sudo apt update
    sudo apt install -y build-essential
else
    echo "Checked: g++ compiler is already installed."
fi

# 2. Ensure data directory and necessary text files exist
echo "Ensuring data directories and required text files exist..."
mkdir -p data
touch data/colonists.txt
touch data/resources.txt
touch data/structures.txt

# 3. Make the build script executable
chmod +x build.sh

echo ""
echo "Installation and setup complete!"
echo "You can now build the project by running: ./build.sh"
