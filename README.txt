SMART CAMPUS NAVIGATION AND FACILITY ANALYTICS SYSTEM

Purpose
-------
This C++17 program loads campus buildings, walkway connections, facilities, and
departments. Milestone 1 provides facility lookup through a custom hash table,
an undirected campus graph with breadth-first reachability, and an academic
department general tree. Records and expected results are never hard-coded.

Build and Run
-------------
Run these commands from the project root:

  g++ -std=c++17 -Wall -Wextra -Wpedantic -Iinclude src/*.cpp -o campus
  ./campus

With no argument, the program reads the four files from data/. To use another
directory containing buildings.txt, connections.txt, facilities.txt, and
departments.txt, pass that directory as the only argument:

  ./campus testdata/integration_validation
  ./campus testdata/hash_collision

The files are always loaded in this order: buildings, connections, facilities,
then departments. Every load prints an accepted/rejected summary. Missing files
and invalid records print diagnostics but do not terminate the program.

Milestone 1 Menu
----------------
  1  View all facilities
  2  Search facility by ID
  3  View campus graph
  4  Display graph counts and degrees
  5  Find reachable buildings (BFS)
  6  Add department
  7  Search department
  8  Display department hierarchy
  9  Run preorder traversal
  0  Exit

The menu reads complete lines, accepts names containing spaces, and returns to
the menu after invalid input.

Directory Layout
----------------
  include/                  Header files for all modules
  src/                      Implementations and main.cpp
  data/                     Four instructor-provided sample files
  testdata/facility_collision/
                             Member A's focused hash-collision fixture
  testdata/facility_validation/
                             Member A's facility-validation fixture
  testdata/facility_tests.cpp
                             Focused FacilityDatabase assertion driver
  testdata/hash_collision/  Custom hash collision and duplicate-ID case
  testdata/integration_validation/
                             Cross-module invalid-record cases
  testdata/tree_*/          Empty, root-only, and validation tree cases
  README.txt                Build, run, and layout instructions
  CONTRIBUTIONS.txt         Member ownership and shared work
  design_document.txt       Data-structure and integration design
  test_results.txt          Milestone 1 tests and recorded results
  .gitignore                Excludes executables, objects, and editor files

Implementation Overview
-----------------------
FacilityDatabase retains accepted facilities in file order and uses the custom
FacilityHashTable for ID lookup. The table uses polynomial hashing and separate
chaining. CampusGraph uses an adjacency list; it preserves building and neighbor
insertion order and performs BFS with a queue. DepartmentTree is a general tree
whose nodes own a dynamic list of children through unique_ptr.

Before submission, build and run these same commands on a UNT CSE CELL machine
and record that separate environment check in test_results.txt.
