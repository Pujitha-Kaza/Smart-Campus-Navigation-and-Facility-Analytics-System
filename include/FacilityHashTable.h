/*
 * File: FacilityHashTable.h
 * Purpose: Declares a separate-chaining table keyed by normalized facility ID.
 * Author: Charvi
 */

#ifndef FACILITY_HASH_TABLE_H
#define FACILITY_HASH_TABLE_H

#include "Facility.h"

#include <cstddef>
#include <vector>

// Stores facilities in dynamically growing buckets and tracks accepted collisions.
class FacilityHashTable {
public:
    // Creates a table with the requested number of buckets.
    explicit FacilityHashTable(std::size_t initialCapacity = 11);

    // Inserts a facility, returning false without counting collisions for duplicate IDs.
    bool insert(const Facility& facility);

    // Finds a facility by normalized ID using its hash bucket.
    const Facility* find(const std::string& normalizedId) const;

    // Reports the number of buckets.
    std::size_t capacity() const;

    // Reports the number of unique facilities.
    std::size_t size() const;

    // Reports accepted insertions into previously nonempty buckets.
    std::size_t collisionCount() const;

private:
    std::vector<std::vector<Facility>> buckets;
    std::size_t entryCount;
    std::size_t collisions;

    // Computes polynomial hash h=(h*31+byte) mod capacity over every key character.
    std::size_t hash(const std::string& normalizedId) const;
};

#endif
