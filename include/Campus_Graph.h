#ifndef CAMPUSGRAPH_H
#define CAMPUSGRAPH_H

#include <string>
#include <vector>

using namespace std;

class CampusGraph {
  private:
    vector<string> buildings;
    vector<vector<int>> adjacency;
    int findBuilding(const string& name) const;

  public:
    CampusGraph();
    bool loadBuildings(const string& filename);
    bool loadConnections(const string& filename);
    void displayGraph() const;
    void displayStatistics() const;
    void bfsReachability() const;
};

#endif
