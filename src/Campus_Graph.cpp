/*
 * File: Campus_Graph.cpp
 * Purpose: Implements validated graph loading, statistics, and BFS reachability.
 * Author: Pujitha; integration validation by Gonul
 */

#include "Campus_Graph.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <queue>

CampusGraph::CampusGraph() = default;

std::string CampusGraph::trim(const std::string& text) {
    std::size_t first = 0;
    while (first < text.size()
           && std::isspace(static_cast<unsigned char>(text[first])) != 0) {
        ++first;
    }

    std::size_t last = text.size();
    while (last > first
           && std::isspace(static_cast<unsigned char>(text[last - 1])) != 0) {
        --last;
    }
    return text.substr(first, last - first);
}

std::string CampusGraph::normalize(const std::string& text) {
    std::string result = trim(text);
    for (char& character : result) {
        character = static_cast<char>(
            std::tolower(static_cast<unsigned char>(character)));
    }
    return result;
}

std::string CampusGraph::filenameOnly(const std::string& path) {
    const std::size_t separator = path.find_last_of("/\\");
    return separator == std::string::npos ? path : path.substr(separator + 1);
}

int CampusGraph::findBuilding(const std::string& name) const {
    const std::string target = normalize(name);
    for (std::size_t index = 0; index < buildings.size(); ++index) {
        if (normalize(buildings[index]) == target) {
            return static_cast<int>(index);
        }
    }
    return -1;
}

bool CampusGraph::loadBuildings(const std::string& filename) {
    return loadBuildings(filename, std::cout, std::cerr);
}

bool CampusGraph::loadBuildings(const std::string& filename,
                                std::ostream& output,
                                std::ostream& errors) {
    buildings.clear();
    adjacency.clear();
    const std::string shownFilename = filenameOnly(filename);
    std::ifstream input(filename);
    if (!input.is_open()) {
        errors << "Error: could not open " << shownFilename << ".\n";
        output << shownFilename << ": 0 accepted, 0 rejected\n";
        return false;
    }

    std::size_t accepted = 0;
    std::size_t rejected = 0;
    std::size_t lineNumber = 0;
    std::string line;
    while (std::getline(input, line)) {
        ++lineNumber;
        const std::string cleaned = trim(line);
        if (cleaned.empty() || cleaned.front() == '#') {
            continue;
        }
        if (cleaned.find('|') != std::string::npos) {
            ++rejected;
            errors << "Warning: " << shownFilename << " line " << lineNumber
                   << ": expected exactly 1 field.\n";
            continue;
        }
        if (findBuilding(cleaned) != -1) {
            ++rejected;
            errors << "Warning: " << shownFilename << " line " << lineNumber
                   << ": duplicate building \"" << cleaned << "\".\n";
            continue;
        }
        buildings.push_back(cleaned);
        adjacency.emplace_back();
        ++accepted;
    }

    output << shownFilename << ": " << accepted << " accepted, "
           << rejected << " rejected\n";
    return true;
}

bool CampusGraph::loadConnections(const std::string& filename) {
    return loadConnections(filename, std::cout, std::cerr);
}

bool CampusGraph::loadConnections(const std::string& filename,
                                  std::ostream& output,
                                  std::ostream& errors) {
    for (std::vector<int>& neighbors : adjacency) {
        neighbors.clear();
    }

    const std::string shownFilename = filenameOnly(filename);
    std::ifstream input(filename);
    if (!input.is_open()) {
        errors << "Error: could not open " << shownFilename << ".\n";
        output << shownFilename << ": 0 accepted, 0 rejected\n";
        return false;
    }

    std::size_t accepted = 0;
    std::size_t rejected = 0;
    std::size_t lineNumber = 0;
    std::string line;
    while (std::getline(input, line)) {
        ++lineNumber;
        const std::string cleaned = trim(line);
        if (cleaned.empty() || cleaned.front() == '#') {
            continue;
        }

        const std::size_t divider = line.find('|');
        std::string reason;
        if (divider == std::string::npos
            || line.find('|', divider + 1) != std::string::npos) {
            reason = "expected exactly 2 fields";
        }

        std::string buildingA;
        std::string buildingB;
        int indexA = -1;
        int indexB = -1;
        if (reason.empty()) {
            buildingA = trim(line.substr(0, divider));
            buildingB = trim(line.substr(divider + 1));
            if (buildingA.empty() || buildingB.empty()) {
                reason = "building names must not be empty";
            } else {
                indexA = findBuilding(buildingA);
                indexB = findBuilding(buildingB);
                if (indexA == -1 || indexB == -1) {
                    const std::string unknown = indexA == -1 ? buildingA : buildingB;
                    reason = "unknown building \"" + unknown + "\"";
                } else if (indexA == indexB) {
                    reason = "self-connection is not allowed";
                } else {
                    const std::vector<int>& neighbors = adjacency[indexA];
                    if (std::find(neighbors.begin(), neighbors.end(), indexB)
                        != neighbors.end()) {
                        reason = "duplicate connection";
                    }
                }
            }
        }

        if (!reason.empty()) {
            ++rejected;
            errors << "Warning: " << shownFilename << " line " << lineNumber
                   << ": " << reason << ".\n";
            continue;
        }

        adjacency[indexA].push_back(indexB);
        adjacency[indexB].push_back(indexA);
        ++accepted;
    }

    output << shownFilename << ": " << accepted << " accepted, "
           << rejected << " rejected\n";
    return true;
}

void CampusGraph::displayGraph() const {
    displayGraph(std::cout);
}

void CampusGraph::displayGraph(std::ostream& output) const {
    if (buildings.empty()) {
        output << "No buildings are loaded.\n";
        return;
    }
    for (std::size_t index = 0; index < buildings.size(); ++index) {
        output << buildings[index] << ": ";
        if (adjacency[index].empty()) {
            output << "(no connections)";
        } else {
            for (std::size_t neighborIndex = 0;
                 neighborIndex < adjacency[index].size();
                 ++neighborIndex) {
                output << buildings[adjacency[index][neighborIndex]];
                if (neighborIndex + 1 < adjacency[index].size()) {
                    output << ", ";
                }
            }
        }
        output << '\n';
    }
}

void CampusGraph::displayStatistics() const {
    displayStatistics(std::cout);
}

void CampusGraph::displayStatistics(std::ostream& output) const {
    std::size_t doubledConnections = 0;
    for (const std::vector<int>& neighbors : adjacency) {
        doubledConnections += neighbors.size();
    }
    output << "Total buildings: " << buildings.size() << '\n';
    output << "Total connections: " << doubledConnections / 2 << '\n';
    output << "Degrees:\n";
    for (std::size_t index = 0; index < buildings.size(); ++index) {
        output << buildings[index] << ": " << adjacency[index].size() << '\n';
    }
}

void CampusGraph::bfsReachability() const {
    bfsReachability(std::cin, std::cout);
}

void CampusGraph::bfsReachability(std::istream& input, std::ostream& output) const {
    if (buildings.empty()) {
        output << "No buildings are loaded.\n";
        return;
    }

    output << "Enter starting building: ";
    std::string startBuilding;
    std::getline(input, startBuilding);
    if (trim(startBuilding).empty()) {
        output << "Starting building cannot be blank.\n";
        return;
    }

    const int start = findBuilding(startBuilding);
    if (start == -1) {
        output << "Building \"" << trim(startBuilding) << "\" was not found.\n";
        return;
    }

    std::vector<bool> visited(buildings.size(), false);
    std::vector<int> distance(buildings.size(), -1);
    std::queue<int> pending;
    visited[start] = true;
    distance[start] = 0;
    pending.push(start);
    std::size_t reachableCount = 0;

    while (!pending.empty()) {
        const int current = pending.front();
        pending.pop();
        ++reachableCount;
        output << buildings[current] << " (distance " << distance[current] << ")\n";
        for (int neighbor : adjacency[current]) {
            if (!visited[neighbor]) {
                visited[neighbor] = true;
                distance[neighbor] = distance[current] + 1;
                pending.push(neighbor);
            }
        }
    }
    output << "Reachable: " << reachableCount << " of " << buildings.size()
           << " buildings\n";
}

const std::vector<std::string>& CampusGraph::getBuildings() const {
    return buildings;
}
