# Smart Campus Navigation and Facility Analytics System

The Milestone 1 C++17 application integrates a custom facility-ID hash table,
an undirected campus graph with BFS reachability, and a department general tree.

Build and run from the repository root:

```sh
g++ -std=c++17 -Wall -Wextra -Wpedantic -Iinclude src/*.cpp -o campus
./campus
```

An optional data-directory argument loads another complete four-file data set:

```sh
./campus testdata/integration_validation
```

See `README.txt` for the required menu, complete usage, and directory layout.
