#include "Campus_Graph.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

CampusGraph::CampusGraph() {}

int CampusGraph::findBuilding(const string& name) const {
  for (int i = 0; i < buildings.size(); i++) {
    if (buildings[i] == name) {
      return i;
    }
  }
  return -1;
}

bool CampusGraph::loadFromFile(const string& filename) {
  ifstream input(filename);
  if (!input) {
    return false;
  }
  string line;
  while (getline(input, line)) {
    if (line.empty() || line[0] == "#") {
      continue;
    }
    size_t divider = line.find('|');
    if (divider == string::npos) {
      continue;
    }
    size_t divider = line.find('|');
	if (divider == string::npos) {
		continue;
	}
	string buildingA = line.substr(0, divider);
	string buildingB = line.substr(divider + 1);
	int indexA = findBuilding(buildingA);
	if (indexA == -1) {
		buildings.push_back(buildingA);
		adjacency.push_back(vector<int>());
		indexA = buildings.size() - 1;
	}
	int indexB = findBuilding(buildingB);
	if (indexB == -1) {
		buildings.push_back(buildingB);
		adjacency.push_back(vector<int>());
		indexB = buildings.size() - 1;
	}
	adjacency[indexA].push_back(indexB);
	adjacency[indexB].push_back(indexA);
  }
  input.close();
  return true;
}

void CampusGraph::displayGraph() const {
	if (buildings.empty()) {
		cout << "No buildings in campus graph." << endl;
		return;
	}
	cout << "\nCampus Graph\n";
	for (int i = 0; i < buildings.size(); i++) {
		cout << buildings[i] << ": ";
		if (adjacency[i].empty()) {
			cout << "(No Connections)";
		}
		else {
			for (int j = 0; j < adjacency[i].size(); j++) {
				cout << buildings[adjacency[i][j]];
				if (j < adjacency[i].size() - 1) {
					cout << ", ";
				}
			}
		}
		cout << endl;
	}
}

void CampusGraph::displayStatistics() const {
	int totalConnections = 0;
	for (int i = 0; i < adjacency.size(); i++) {
		totalConnections += adjacency[i].size();
	}
	totalConnections /= 2;
	cout << "\nGraph Statistics\n";
	cout << "Total Buildings: " << buildings.size() << endl;
	cout << "Total Connections: " << totalConnections << endl;
	cout << "\nDegrees\n";
	for (int i = 0; i < buildings.size(); i++) {
		cout << buildings[i] << ": " << adjacency[i].size() << endl;
	}
}
