/*
 * File: Campus_Graph.h
 * Purpose: Declares the campus adjacency-list graph and BFS operations.
 * Author: Pujitha; integration validation by Gonul
 */

#ifndef CAMPUS_GRAPH_H
#define CAMPUS_GRAPH_H

#include <iosfwd>
#include <string>
#include <vector>

// Stores buildings in insertion order and undirected connections in neighbor order.
class CampusGraph {
public:
    CampusGraph();

    // Loads buildings and prints a filename-based accepted/rejected summary.
    bool loadBuildings(const std::string& filename);
    bool loadBuildings(const std::string& filename,
                       std::ostream& output,
                       std::ostream& errors);

    // Loads undirected connections and prints validation warnings and a summary.
    bool loadConnections(const std::string& filename);
    bool loadConnections(const std::string& filename,
                         std::ostream& output,
                         std::ostream& errors);

    // Displays the adjacency list in the required insertion order.
    void displayGraph() const;
    void displayGraph(std::ostream& output) const;

    // Displays graph totals and the degree of every building.
    void displayStatistics() const;
    void displayStatistics(std::ostream& output) const;

    // Prompts for a start and displays BFS visit order and distance.
    void bfsReachability() const;
    void bfsReachability(std::istream& input, std::ostream& output) const;

    // Exposes accepted building spellings for facility-reference validation.
    const std::vector<std::string>& getBuildings() const;

private:
    std::vector<std::string> buildings;
    std::vector<std::vector<int>> adjacency;

    int findBuilding(const std::string& name) const;
    static std::string normalize(const std::string& text);
    static std::string trim(const std::string& text);
    static std::string filenameOnly(const std::string& path);
};

#endif
