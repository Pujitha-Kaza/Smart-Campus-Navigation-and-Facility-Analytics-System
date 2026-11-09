/*
 * File: Facility.h
 * Purpose: Defines the facility record and shared loading result.
 * Author: Charvi
 */

#ifndef FACILITY_H
#define FACILITY_H

#include <cstddef>
#include <string>

// Holds one validated facility using the accepted building spelling.
struct Facility {
    std::string id;
    std::string name;
    std::string building;
    std::string category;
};

// Reports accepted records, rejected records, and hash diagnostics.
struct FacilityLoadResult {
    std::size_t accepted = 0;
    std::size_t rejected = 0;
    std::size_t capacity = 0;
    std::size_t entries = 0;
    std::size_t collisions = 0;
};

#endif
