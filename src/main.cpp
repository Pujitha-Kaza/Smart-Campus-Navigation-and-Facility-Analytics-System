/*
 * File: main.cpp
 * Purpose: Loads all Milestone 1 data and coordinates the required menu.
 * Author: Gonul (shared-module integration)
 */

#include "Campus_Graph.h"
#include "DepartmentTree.h"
#include "FacilityDatabase.h"

#include <cctype>
#include <iostream>
#include <sstream>
#include <string>

namespace {

std::string trim(const std::string& text) {
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

std::string dataPath(const std::string& directory, const std::string& filename) {
    if (directory.empty()) return filename;
    const char last = directory.back();
    if (last == '/' || last == '\\') return directory + filename;
    return directory + '/' + filename;
}

bool parseMenuChoice(const std::string& input, int& choice) {
    std::istringstream parser(input);
    parser >> choice;
    if (!parser) return false;
    parser >> std::ws;
    return parser.eof() && choice >= 0 && choice <= 9;
}

void printMenu(std::ostream& output) {
    output << "\nSmart Campus Navigation and Facility Analytics\n"
           << "1. View all facilities\n"
           << "2. Search facility by ID\n"
           << "3. View campus graph\n"
           << "4. Display graph counts and degrees\n"
           << "5. Find reachable buildings (BFS)\n"
           << "6. Add department\n"
           << "7. Search department\n"
           << "8. Display department hierarchy\n"
           << "9. Run preorder traversal\n"
           << "0. Exit\n";
}

bool promptLine(const std::string& prompt, std::string& value) {
    std::cout << prompt;
    return static_cast<bool>(std::getline(std::cin, value));
}

void addDepartment(DepartmentTree& departments) {
    std::string departmentName;
    if (!promptLine("Enter department name: ", departmentName)) return;

    std::string parentName;
    if (!departments.empty()
        && !promptLine("Enter parent department: ", parentName)) {
        return;
    }

    std::string errorMessage;
    if (departments.addDepartment(departmentName, parentName, errorMessage)) {
        std::cout << "Department added.\n";
    } else {
        std::cout << "Error: " << errorMessage << '\n';
    }
}

void searchDepartment(const DepartmentTree& departments) {
    std::string departmentName;
    if (!promptLine("Enter department name: ", departmentName)) return;
    if (trim(departmentName).empty()) {
        std::cout << "Department name cannot be blank.\n";
        return;
    }

    DepartmentInfo info;
    if (!departments.getDepartmentInfo(departmentName, info)) {
        std::cout << "Department \"" << trim(departmentName)
                  << "\" was not found.\n";
        return;
    }
    std::cout << "Name: " << info.name << '\n'
              << "Parent: " << info.parentName << '\n'
              << "Direct children: " << info.directChildCount << '\n'
              << "Depth: " << info.depth << '\n';
}

}  // namespace

int main(int argc, char* argv[]) {
    if (argc > 2) {
        std::cerr << "Usage: " << argv[0] << " [data-directory]\n";
        return 1;
    }

    const std::string dataDirectory = argc == 2 ? argv[1] : "data";
    CampusGraph graph;
    FacilityDatabase facilities;
    DepartmentTree departments;

    // The required load order is buildings, connections, facilities, departments.
    graph.loadBuildings(dataPath(dataDirectory, "buildings.txt"));
    graph.loadConnections(dataPath(dataDirectory, "connections.txt"));
    facilities.loadFromFile(dataPath(dataDirectory, "facilities.txt"),
                            graph.getBuildings(), std::cout, std::cerr);
    departments.loadFromFile(dataPath(dataDirectory, "departments.txt"),
                             std::cout, std::cerr);

    while (true) {
        printMenu(std::cout);
        std::cout << "Choose an option: ";

        std::string input;
        if (!std::getline(std::cin, input)) {
            std::cout << "\nInput closed. Exiting.\n";
            break;
        }

        int choice = 0;
        if (!parseMenuChoice(input, choice)) {
            std::cout << "Invalid choice. Enter a number from 0 to 9.\n";
            continue;
        }

        switch (choice) {
            case 1:
                facilities.display(std::cout);
                break;
            case 2: {
                std::string id;
                if (promptLine("Enter facility ID: ", id)) {
                    facilities.searchById(id, std::cout);
                }
                break;
            }
            case 3:
                graph.displayGraph(std::cout);
                break;
            case 4:
                graph.displayStatistics(std::cout);
                break;
            case 5:
                graph.bfsReachability(std::cin, std::cout);
                break;
            case 6:
                addDepartment(departments);
                break;
            case 7:
                searchDepartment(departments);
                break;
            case 8:
                departments.displayHierarchy(std::cout);
                break;
            case 9:
                departments.displayPreorder(std::cout);
                break;
            case 0:
                std::cout << "Goodbye.\n";
                return 0;
        }

        if (!std::cin) {
            std::cout << "\nInput closed. Exiting.\n";
            break;
        }
    }
    return 0;
}
