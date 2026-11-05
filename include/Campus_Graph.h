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
    bool loadFromFile(const string& filename);
    void displayGraph() const;
    void displayStatistics() const;
};

#endif
