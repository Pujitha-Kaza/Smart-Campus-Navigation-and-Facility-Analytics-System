/*
 * File: DepartmentTree.h
 * Purpose: Declares the general tree used for the campus department hierarchy.
 * Author: Gonul
 */

#ifndef DEPARTMENT_TREE_H
#define DEPARTMENT_TREE_H

#include <cstddef>
#include <iosfwd>
#include <memory>
#include <string>
#include <vector>

// Stores the accepted and rejected record counts from departments.txt.
struct DepartmentLoadResult {
    std::size_t accepted = 0;
    std::size_t rejected = 0;
};

// Provides the details required when a department search succeeds.
struct DepartmentInfo {
    std::string name;
    std::string parentName;
    std::size_t directChildCount = 0;
    std::size_t depth = 0;
};

// Represents a general tree in which each department may have any number of children.
class DepartmentTree {
public:
    // Creates an empty department tree.
    DepartmentTree();

    // Releases the tree through its owned smart pointers.
    ~DepartmentTree();

    DepartmentTree(const DepartmentTree&) = delete;
    DepartmentTree& operator=(const DepartmentTree&) = delete;

    // Clears the current tree, loads a department file, and prints warnings and a summary.
    DepartmentLoadResult loadFromFile(const std::string& filename,
                                      std::ostream& output,
                                      std::ostream& errors);

    // Adds a root to an empty tree or a child under an existing parent.
    bool addDepartment(const std::string& departmentName,
                       const std::string& parentName,
                       std::string& errorMessage);

    // Finds a department case-insensitively and fills the requested search details.
    bool getDepartmentInfo(const std::string& departmentName,
                           DepartmentInfo& info) const;

    // Displays the hierarchy in preorder with four spaces per depth level.
    void displayHierarchy(std::ostream& output) const;

    // Displays the preorder sequence without indentation.
    void displayPreorder(std::ostream& output) const;

    // Reports whether the tree contains no departments.
    bool empty() const;

    // Returns the number of departments currently stored.
    std::size_t size() const;

    // Removes all departments from memory.
    void clear();

private:
    // Stores one department and owns all of its child nodes.
    class DepartmentNode {
    public:
        // Creates a node with a non-owning pointer to its parent.
        DepartmentNode(std::string nodeName, DepartmentNode* parentNode);

        std::string name;
        DepartmentNode* parent;
        std::vector<std::unique_ptr<DepartmentNode>> children;
    };

    std::unique_ptr<DepartmentNode> root;
    std::size_t departmentCount;

    // Trims whitespace, including carriage returns, from both ends of text.
    static std::string trim(const std::string& text);

    // Compares two strings without regard to ASCII letter capitalization.
    static bool equalsIgnoreCase(const std::string& left,
                                 const std::string& right);

    // Splits a complete input line while preserving empty fields.
    static std::vector<std::string> split(const std::string& line,
                                          char delimiter);

    // Returns only the filename portion of a path for diagnostics.
    static std::string filenameOnly(const std::string& path);

    // Recursively searches for a node in a mutable tree.
    static DepartmentNode* findNode(DepartmentNode* node,
                                    const std::string& departmentName);

    // Recursively searches for a node in a read-only tree.
    static const DepartmentNode* findNode(const DepartmentNode* node,
                                          const std::string& departmentName);

    // Recursively prints a hierarchy with depth-based indentation.
    static void displayHierarchy(const DepartmentNode* node,
                                 std::size_t depth,
                                 std::ostream& output);

    // Recursively prints names in preorder as one sequence.
    static void displayPreorder(const DepartmentNode* node,
                                bool& firstName,
                                std::ostream& output);
};

#endif
