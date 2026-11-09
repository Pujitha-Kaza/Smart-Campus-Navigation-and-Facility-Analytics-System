/*
 * File: FacilityDatabase.h
 * Purpose: Declares validated facility loading, ordered display, and ID search.
 * Author: Charvi
 */

#ifndef FACILITY_DATABASE_H
#define FACILITY_DATABASE_H

#include "Facility.h"
#include "FacilityHashTable.h"

#include <iosfwd>
#include <string>
#include <vector>

// Owns accepted facilities in file order and indexes their IDs in a custom table.
class FacilityDatabase {
public:
    // Creates an empty database with the named default table capacity.
    FacilityDatabase();

    // Loads and validates facilities against the accepted building spellings.
    FacilityLoadResult loadFromFile(const std::string& filename,
                                    const std::vector<std::string>& buildings,
                                    std::ostream& output,
                                    std::ostream& errors);

    // Displays all accepted facilities in their original accepted order.
    void display(std::ostream& output) const;

    // Validates and searches a user-supplied ID through the custom hash table.
    bool searchById(const std::string& id, std::ostream& output) const;

    // Reports the number of accepted facilities.
    std::size_t size() const;

    // Reports the configured hash-table capacity.
    std::size_t capacity() const;

    // Reports the accepted collision count.
    std::size_t collisionCount() const;

private:
    static constexpr std::size_t kInitialCapacity = 11;
    std::vector<Facility> facilities;
    FacilityHashTable index;

    // Trims only the required outer whitespace characters.
    static std::string trim(const std::string& text);

    // Converts ASCII letters to uppercase for identifiers and comparisons.
    static std::string uppercase(const std::string& text);

    // Checks the required F followed by exactly three decimal digits format.
    static bool validId(const std::string& id);

    // Splits a complete record while preserving empty fields.
    static std::vector<std::string> split(const std::string& line);

    // Extracts a portable filename for diagnostics.
    static std::string filenameOnly(const std::string& path);
};

#endif
