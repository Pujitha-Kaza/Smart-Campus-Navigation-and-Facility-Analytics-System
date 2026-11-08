/*
 * File: facility_tests.cpp
 * Purpose: Exercises facility validation, hash behavior, search, and empty states.
 * Author: Charvi
 */

#include "FacilityDatabase.h"

#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

// Writes exact CRLF records to cover Windows line endings and outer whitespace.
void writeWindowsFixture(const std::filesystem::path& path) {
    std::ofstream file(path, std::ios::binary);
    file << "  F011 \t|  Spaced Name  |  library \t|  Study  \r\n";
}

// Loads the validation fixture and asserts the core parser requirements.
void testValidationAndSearch() {
    FacilityDatabase database;
    std::ostringstream summary;
    std::ostringstream warnings;
    const FacilityLoadResult result = database.loadFromFile(
        "testdata/facility_validation/facilities.txt",
        {"Library", "Student Union", "Science Building"}, summary, warnings);
    assert(result.accepted == 5);
    assert(result.rejected == 6);
    assert(result.capacity == 11 && result.entries == 5);
    assert(database.size() == 5);
    assert(warnings.str().find("facilities.txt line 5") != std::string::npos);
    assert(warnings.str().find("facilities.txt line 6") != std::string::npos);
    assert(warnings.str().find("facilities.txt line 7") != std::string::npos);
    assert(warnings.str().find("facilities.txt line 8") != std::string::npos);
    assert(warnings.str().find("facilities.txt line 9") != std::string::npos);
    assert(warnings.str().find("facilities.txt line 10") != std::string::npos);
    assert(summary.str().find("hash capacity 11") != std::string::npos);

    std::ostringstream display;
    database.display(display);
    assert(display.str().find("ID: F001 | Name: Shared Name | Building: Library") != std::string::npos);
    assert(display.str().find("ID: F008 | Name: Shared Name | Building: Student Union") != std::string::npos);
    assert(display.str().find("ID: F002 | Name: Lowercase ID | Building: Library") != std::string::npos);
    assert(display.str().find("Hash # in name") != std::string::npos);
    assert(display.str().find("Total facilities: 5") != std::string::npos);
    assert(display.str().find("Duplicate ID") == std::string::npos);

    std::ostringstream foundUpper;
    assert(database.searchById("F002", foundUpper));
    assert(foundUpper.str().find("F002") != std::string::npos);
    std::ostringstream foundLower;
    assert(database.searchById(" f002 ", foundLower));
    assert(foundLower.str().find("F002") != std::string::npos);

    FacilityDatabase suppliedData;
    std::ostringstream suppliedSummary;
    std::ostringstream suppliedWarnings;
    suppliedData.loadFromFile("data/facilities.txt",
                              {"Library", "Student Union", "Science Building",
                               "Engineering Building", "Business Building", "Gym",
                               "Observatory"},
                              suppliedSummary, suppliedWarnings);
    std::ostringstream requiredF004;
    assert(suppliedData.searchById("F004", requiredF004));
    assert(requiredF004.str().find("F004") != std::string::npos);
    std::ostringstream requiredLowerF004;
    assert(suppliedData.searchById("f004", requiredLowerF004));
    assert(requiredLowerF004.str().find("F004") != std::string::npos);
    std::ostringstream invalid;
    assert(!database.searchById("004", invalid));
    assert(invalid.str().find("F followed by exactly three digits") != std::string::npos);
    std::ostringstream missing;
    assert(!database.searchById("f999", missing));
    assert(missing.str().find("F999 was not found") != std::string::npos);
}

// Proves that colliding accepted IDs increment the counter and duplicates do not.
void testCollisionsAndDuplicateInsert() {
    FacilityDatabase database;
    std::ostringstream summary;
    std::ostringstream warnings;
    const FacilityLoadResult result = database.loadFromFile(
        "testdata/facility_collision/facilities.txt", {"Library"}, summary, warnings);
    assert(result.accepted == 2 && result.rejected == 0);
    assert(result.collisions == 1);

    FacilityHashTable table(11);
    const Facility first{"F009", "First", "Library", "Test"};
    const Facility second{"F010", "Second", "Library", "Test"};
    assert(table.insert(first));
    assert(table.insert(second));
    assert(table.collisionCount() == 1);
    assert(!table.insert(first));
    assert(table.collisionCount() == 1);
}

// Verifies exact Windows endings and missing-file behavior.
void testLineEndingsAndMissingFile() {
    const std::filesystem::path fixture = "testdata/facility_validation/windows.txt";
    writeWindowsFixture(fixture);
    FacilityDatabase database;
    std::ostringstream summary;
    std::ostringstream warnings;
    const FacilityLoadResult result = database.loadFromFile(
        fixture.string(), {"Library"}, summary, warnings);
    assert(result.accepted == 1 && result.rejected == 0);
    std::ostringstream display;
    database.display(display);
    assert(display.str().find("F011 | Name: Spaced Name | Building: Library") != std::string::npos);
    std::filesystem::remove(fixture);

    std::ostringstream missingSummary;
    std::ostringstream missingError;
    const FacilityLoadResult missing = database.loadFromFile(
        "testdata/no_such_folder/facilities.txt", {"Library"}, missingSummary, missingError);
    assert(missing.accepted == 0 && missing.rejected == 0);
    assert(missing.entries == 0 && missing.capacity == 11);
    assert(missingError.str().find("facilities.txt") != std::string::npos);
}

// Verifies empty display and invalid and absent searches.
void testEmptyDatabase() {
    FacilityDatabase database;
    std::ostringstream display;
    database.display(display);
    assert(display.str().find("No facilities are loaded.") != std::string::npos);
    assert(display.str().find("Total facilities: 0") != std::string::npos);
    std::ostringstream invalid;
    assert(!database.searchById("004", invalid));
    std::ostringstream absent;
    assert(!database.searchById("F999", absent));
    assert(absent.str().find("F999 was not found") != std::string::npos);
}

// Runs the focused facility test suite.
int main() {
    testValidationAndSearch();
    testCollisionsAndDuplicateInsert();
    testLineEndingsAndMissingFile();
    testEmptyDatabase();
    std::cout << "All FacilityDatabase tests passed.\n";
    return 0;
}
