/*
 * File: FacilityDatabase.cpp
 * Purpose: Implements facility file validation, ordered display, and ID search.
 * Author: Charvi
 */

#include "FacilityDatabase.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <ostream>

FacilityDatabase::FacilityDatabase() : facilities(), index(kInitialCapacity) {}

// Loads each valid facility once and reports line-specific warnings and diagnostics.
FacilityLoadResult FacilityDatabase::loadFromFile(
    const std::string& filename,
    const std::vector<std::string>& buildings,
    std::ostream& output,
    std::ostream& errors) {
    facilities.clear();
    index = FacilityHashTable(kInitialCapacity);
    FacilityLoadResult result;
    const std::string shownFilename = filenameOnly(filename);
    std::ifstream input(filename);
    if (!input.is_open()) {
        errors << "Error: could not open " << shownFilename << ".\n";
        output << shownFilename << ": 0 accepted, 0 rejected; hash capacity "
               << index.capacity() << ", entries " << index.size()
               << ", collisions " << index.collisionCount() << '\n';
        result.capacity = index.capacity();
        return result;
    }

    std::vector<std::string> acceptedBuildings;
    for (const std::string& building : buildings) {
        const std::string cleaned = trim(building);
        if (!cleaned.empty()) acceptedBuildings.push_back(cleaned);
    }

    std::string line;
    std::size_t lineNumber = 0;
    while (std::getline(input, line)) {
        ++lineNumber;
        const auto firstNonWhitespace = std::find_if_not(
            line.begin(), line.end(), [](unsigned char character) {
                return std::isspace(character) != 0;
            });
        if (firstNonWhitespace == line.end() || *firstNonWhitespace == '#') continue;

        const std::vector<std::string> fields = split(line);
        std::string reason;
        Facility facility;
        if (fields.size() != 4) {
            reason = "expected exactly 4 fields";
        } else {
            facility.id = uppercase(trim(fields[0]));
            facility.name = trim(fields[1]);
            const std::string requestedBuilding = trim(fields[2]);
            facility.category = trim(fields[3]);
            if (!validId(facility.id)) {
                reason = "facility ID must be F followed by exactly three digits";
            } else if (facility.name.empty() || requestedBuilding.empty() || facility.category.empty()) {
                reason = "facility name, building name, and category must not be empty";
            } else if (index.find(facility.id) != nullptr) {
                reason = "duplicate facility ID " + facility.id;
            } else {
                for (const std::string& building : acceptedBuildings) {
                    if (uppercase(building) == uppercase(requestedBuilding)) {
                        facility.building = building;
                        break;
                    }
                }
                if (facility.building.empty()) reason = "unknown building " + requestedBuilding;
            }
        }

        if (!reason.empty()) {
            ++result.rejected;
            errors << "Warning: " << shownFilename << " line " << lineNumber
                   << ": " << reason << ".\n";
            continue;
        }
        index.insert(facility);
        facilities.push_back(facility);
        ++result.accepted;
    }

    result.capacity = index.capacity();
    result.entries = index.size();
    result.collisions = index.collisionCount();
    output << shownFilename << ": " << result.accepted << " accepted, "
           << result.rejected << " rejected; hash capacity " << result.capacity
           << ", entries " << result.entries << ", collisions " << result.collisions << '\n';
    return result;
}

void FacilityDatabase::display(std::ostream& output) const {
    if (facilities.empty()) output << "No facilities are loaded.\n";
    for (const Facility& facility : facilities) {
        output << "ID: " << facility.id << " | Name: " << facility.name
               << " | Building: " << facility.building
               << " | Category: " << facility.category << '\n';
    }
    output << "Total facilities: " << facilities.size() << '\n';
}

bool FacilityDatabase::searchById(const std::string& id, std::ostream& output) const {
    const std::string normalizedId = uppercase(trim(id));
    if (!validId(normalizedId)) {
        output << "Invalid facility ID. ID must be F followed by exactly three digits.\n";
        return false;
    }
    const Facility* facility = index.find(normalizedId);
    if (facility == nullptr) {
        output << "Facility " << normalizedId << " was not found.\n";
        return false;
    }
    output << "ID: " << facility->id << " | Name: " << facility->name
           << " | Building: " << facility->building
           << " | Category: " << facility->category << '\n';
    return true;
}

std::size_t FacilityDatabase::size() const { return facilities.size(); }
std::size_t FacilityDatabase::capacity() const { return index.capacity(); }
std::size_t FacilityDatabase::collisionCount() const { return index.collisionCount(); }

std::string FacilityDatabase::trim(const std::string& text) {
    const std::string whitespace = " \t\r";
    const std::size_t first = text.find_first_not_of(whitespace);
    if (first == std::string::npos) return "";
    const std::size_t last = text.find_last_not_of(whitespace);
    return text.substr(first, last - first + 1);
}

std::string FacilityDatabase::uppercase(const std::string& text) {
    std::string result = text;
    for (char& character : result) {
        character = static_cast<char>(std::toupper(static_cast<unsigned char>(character)));
    }
    return result;
}

bool FacilityDatabase::validId(const std::string& id) {
    return id.size() == 4 && id[0] == 'F'
        && std::isdigit(static_cast<unsigned char>(id[1])) != 0
        && std::isdigit(static_cast<unsigned char>(id[2])) != 0
        && std::isdigit(static_cast<unsigned char>(id[3])) != 0;
}

std::vector<std::string> FacilityDatabase::split(const std::string& line) {
    std::vector<std::string> fields;
    std::size_t begin = 0;
    for (std::size_t position = 0; position <= line.size(); ++position) {
        if (position == line.size() || line[position] == '|') {
            fields.push_back(line.substr(begin, position - begin));
            begin = position + 1;
        }
    }
    return fields;
}

std::string FacilityDatabase::filenameOnly(const std::string& path) {
    const std::size_t separator = path.find_last_of("/\\");
    return separator == std::string::npos ? path : path.substr(separator + 1);
}
