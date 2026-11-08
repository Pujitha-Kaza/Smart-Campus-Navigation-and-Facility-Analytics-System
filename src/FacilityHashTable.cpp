/*
 * File: FacilityHashTable.cpp
 * Purpose: Implements the custom separate-chaining facility-ID index.
 * Author: Charvi
 */

#include "FacilityHashTable.h"

#include <stdexcept>

// Creates a table with at least one bucket.
FacilityHashTable::FacilityHashTable(std::size_t initialCapacity)
    : buckets(initialCapacity == 0 ? 1 : initialCapacity), entryCount(0), collisions(0) {}

// Inserts unique IDs and counts a collision only when the destination bucket is occupied.
bool FacilityHashTable::insert(const Facility& facility) {
    std::vector<Facility>& bucket = buckets[hash(facility.id)];
    for (const Facility& entry : bucket) {
        if (entry.id == facility.id) {
            return false;
        }
    }
    if (!bucket.empty()) {
        ++collisions;
    }
    bucket.push_back(facility);
    ++entryCount;
    return true;
}

// Looks only in the ID's computed bucket.
const Facility* FacilityHashTable::find(const std::string& normalizedId) const {
    const std::vector<Facility>& bucket = buckets[hash(normalizedId)];
    for (const Facility& entry : bucket) {
        if (entry.id == normalizedId) {
            return &entry;
        }
    }
    return nullptr;
}

// Returns the bucket count.
std::size_t FacilityHashTable::capacity() const { return buckets.size(); }

// Returns the unique entry count.
std::size_t FacilityHashTable::size() const { return entryCount; }

// Returns the number of accepted insertions into occupied buckets.
std::size_t FacilityHashTable::collisionCount() const { return collisions; }

// Hashes every character with the documented polynomial formula modulo capacity.
std::size_t FacilityHashTable::hash(const std::string& normalizedId) const {
    std::size_t value = 0;
    for (unsigned char character : normalizedId) {
        value = (value * 31U + static_cast<std::size_t>(character)) % buckets.size();
    }
    return value;
}
