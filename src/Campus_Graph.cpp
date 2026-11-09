#include "Campus_Graph.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <queue>
#include <algorithm>
#include <cctype>

using namespace std;

CampusGraph::CampusGraph() {}

string CampusGraph::trim(const string& text) const {
	string result = text;
	while (!result.empty() && isspace(result.front())) {
		result.erase(result.begin());
	}
	while (!result.empty() && isspace(result.back())) {
		result.pop_back();
	}
	return result;
}
string CampusGraph::normalize(const string& text) const {
	string result = trim(text);
	if (!result.empty() && result.back() == '\r') { // remove extra hidden character
		result.pop_back();
	}
	for (size_t i = 0; i < result.length(); i++) { // lowercase the input
		result[i] = tolower(result[i]);
	}
	return result;
}

int CampusGraph::findBuilding(const string& name) const { // building search
  string target = normalize(name);
  for (size_t i = 0; i < buildings.size(); i++) {
    if (normalize(buildings[i]) == target) {
      return i;
    }
  }
  return -1;
}

bool CampusGraph::loadBuildings(const string& filename) { // loads all buildings from building.txt
	ifstream input(filename);
	if (!input) {
		cerr << "Error: could not open " << filename << endl;
		return false;
	}
	string line;
	int count = 0;
	int lineNumber = 0;
	while (getline(input, line)) {
		lineNumber++;
		if (!line.empty() && line.back() == '\r') {
			line.pop_back();
		}
		string temp = trim(line);
		if (temp.empty() || temp[0] == '#') {
			continue;
		}
		line = trim(line);
		if (findBuilding(line) != -1) {
			cout << filename << "'s rejected line: " << lineNumber << " (duplicate building)." << endl;
			continue;
		}
		buildings.push_back(line);
		adjacency.push_back(vector<int>());
		count++;
	}
	cout << count << " building(s) loaded." << endl;
	return true;
}
 
bool CampusGraph::loadConnections(const string& filename) { // loads connections from connections.txt
	ifstream input(filename);
	if (!input) {
		cerr << "Error: could not open " << filename << endl;
		return false;
	}
	string line;
	int loaded = 0;
	int rejected = 0;
	int lineNumber = 0;
	while (getline(input, line)) {
		lineNumber++;
		string temp = trim(line);
		if (temp.empty() || temp[0] == '#') {
			continue;
		}
		size_t divider = line.find('|');
		if (divider == string::npos) {
			rejected++;
			continue;
		}
		string buildingA = trim(line.substr(0, divider));
		string buildingB = trim(line.substr(divider + 1));
		int indexA = findBuilding(buildingA);
		int indexB = findBuilding(buildingB);
		if (indexA == -1 || indexB == -1) { // if unknown building is found
			cout << filename << "'s rejected line: " << lineNumber << " (unknown building)." << endl;
			rejected++;
			continue;
		}
		if (indexA == indexB) { // if both buildings are the same thing
			cout << filename << "'s rejected line: " << lineNumber << " (self-connection)." << endl;
			rejected++;
			continue;
		}
		bool duplicate = false;
		for (int neighbor : adjacency[indexA]) { // if building connection is duplicated or reversed
			if (neighbor == indexB) {
				duplicate = true;
				break;
			}
		}
		if (duplicate) {
			cout << filename << "'s rejected line: " << lineNumber << " (duplicate connection)." << endl;
			rejected++;
			continue;
		}
		adjacency[indexA].push_back(indexB);
		adjacency[indexB].push_back(indexA);
		loaded++;
	}
	cout << "\nLoad Summary" << endl;
	cout << loaded << " connection(s) loaded." << endl;
	cout << rejected << " record(s) have been rejected." << endl;
	return true;
}

void CampusGraph::displayGraph() const { // displays graph
	if (buildings.empty()) {
		cout << "No buildings in campus graph." << endl;
		return;
	}
	cout << "\nCampus Graph\n";
	for (size_t i = 0; i < buildings.size(); i++) {
		cout << buildings[i] << ": ";
		if (adjacency[i].empty()) {
			cout << "(no connections)";
		}
		else {
			for (size_t j = 0; j < adjacency[i].size(); j++) {
				cout << buildings[adjacency[i][j]];
				if (j < adjacency[i].size() - 1) {
					cout << ", ";
				}
			}
		}
		cout << endl;
	}
}

void CampusGraph::displayStatistics() const { // displays graph stats
	int totalConnections = 0;
	for (size_t i = 0; i < adjacency.size(); i++) {
		totalConnections += adjacency[i].size();
	}
	totalConnections /= 2;
	cout << "\nGraph Statistics\n";
	cout << "Total Buildings: " << buildings.size() << endl;
	cout << "Total Connections: " << totalConnections << endl;
	cout << "\nDegrees\n";
	for (size_t i = 0; i < buildings.size(); i++) {
		cout << buildings[i] << ": " << adjacency[i].size() << endl;
	}
}

void CampusGraph::bfsReachability() const { // BFS reachability implementation
	string startBuilding;
	cout << "Enter starting building: ";
	getline(cin, startBuilding);
	int start = findBuilding(startBuilding);
	if (start == -1) {
		cout << "Building not found." << endl;
		return;
	}
	vector<bool> visited(buildings.size(), false);
	vector<int> distance(buildings.size(), -1);
	queue<int> q;
	visited[start] = true;
	distance[start] = 0;
	q.push(start);
	int reachableCount = 0;
	cout << "\nReachable Buildings\n";
	while (!q.empty()) {
		int current = q.front();
		q.pop();
		reachableCount++;
		cout << buildings[current] << " (" << distance[current] << ")" << endl;
		for (int neighbor : adjacency[current]) {
			if (!visited[neighbor]) {
				visited[neighbor] = true;
				distance[neighbor] = distance[current] + 1;
				q.push(neighbor);
			}
		}
	}
	cout << "\nReachable: " << reachableCount << " of " << buildings.size() << endl;
}
