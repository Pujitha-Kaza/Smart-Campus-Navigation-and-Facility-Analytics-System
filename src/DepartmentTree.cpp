/*
 * File: DepartmentTree.cpp
 * Purpose: Implements department loading, searching, insertion, and preorder displays.
 * Author: Gonul
 */

#include "DepartmentTree.h"

#include <cctype>
#include <fstream>
#include <ostream>
#include <utility>

// Creates a department node and records its parent without taking ownership of it.
DepartmentTree::DepartmentNode::DepartmentNode(std::string nodeName,
                                                DepartmentNode* parentNode)
    : name(std::move(nodeName)), parent(parentNode) {}

// Initializes an empty general tree.
DepartmentTree::DepartmentTree() : root(nullptr), departmentCount(0) {}

// Smart pointers recursively release every child node.
DepartmentTree::~DepartmentTree() = default;

// Loads and validates department records while preserving accepted insertion order.
DepartmentLoadResult DepartmentTree::loadFromFile(const std::string& filename,
                                                   std::ostream& output,
                                                   std::ostream& errors) {
    clear();
    DepartmentLoadResult result;
    const std::string displayFilename = filenameOnly(filename);
    std::ifstream input(filename);

    if (!input.is_open()) {
        errors << "Error: could not open " << displayFilename << ".\n";
        output << displayFilename << ": 0 accepted, 0 rejected\n";
        return result;
    }

    std::string line;
    std::size_t lineNumber = 0;

    while (std::getline(input, line)) {
        ++lineNumber;
        const std::string trimmedLine = trim(line);

        if (trimmedLine.empty() || trimmedLine.front() == '#') {
            continue;
        }

        const std::vector<std::string> fields = split(line, '|');
        std::string rejectionReason;
        std::string departmentName;
        std::string parentName;

        if (fields.size() != 2) {
            rejectionReason = "expected exactly 2 fields";
        } else {
            departmentName = trim(fields[0]);
            parentName = trim(fields[1]);

            if (departmentName.empty() || parentName.empty()) {
                rejectionReason = "department and parent names must not be empty";
            } else if (findNode(root.get(), departmentName) != nullptr) {
                rejectionReason = "duplicate department \"" + departmentName + "\"";
            } else if (root == nullptr && !equalsIgnoreCase(parentName, "NONE")) {
                rejectionReason = "the first accepted department must use NONE as its parent";
            } else if (root != nullptr && findNode(root.get(), parentName) == nullptr) {
                rejectionReason = "unknown parent \"" + parentName + "\"";
            }
        }

        if (!rejectionReason.empty()) {
            ++result.rejected;
            errors << "Warning: " << displayFilename << " line " << lineNumber
                   << ": " << rejectionReason << ".\n";
            continue;
        }

        if (root == nullptr) {
            root = std::make_unique<DepartmentNode>(departmentName, nullptr);
        } else {
            DepartmentNode* parent = findNode(root.get(), parentName);
            parent->children.push_back(
                std::make_unique<DepartmentNode>(departmentName, parent));
        }

        ++departmentCount;
        ++result.accepted;
    }

    output << displayFilename << ": " << result.accepted << " accepted, "
           << result.rejected << " rejected\n";
    return result;
}

// Adds a department after trimming input and checking root, duplicate, and parent rules.
bool DepartmentTree::addDepartment(const std::string& departmentName,
                                   const std::string& parentName,
                                   std::string& errorMessage) {
    const std::string cleanedName = trim(departmentName);
    const std::string cleanedParent = trim(parentName);
    errorMessage.clear();

    if (cleanedName.empty()) {
        errorMessage = "Department name cannot be blank.";
        return false;
    }

    if (findNode(root.get(), cleanedName) != nullptr) {
        errorMessage = "Department \"" + cleanedName + "\" already exists.";
        return false;
    }

    if (root == nullptr) {
        root = std::make_unique<DepartmentNode>(cleanedName, nullptr);
        departmentCount = 1;
        return true;
    }

    if (cleanedParent.empty()) {
        errorMessage = "Parent department name cannot be blank.";
        return false;
    }

    DepartmentNode* parent = findNode(root.get(), cleanedParent);
    if (parent == nullptr) {
        errorMessage = "Parent department \"" + cleanedParent + "\" was not found.";
        return false;
    }

    parent->children.push_back(
        std::make_unique<DepartmentNode>(cleanedName, parent));
    ++departmentCount;
    return true;
}

// Returns the parent, direct-child count, and depth for a case-insensitive match.
bool DepartmentTree::getDepartmentInfo(const std::string& departmentName,
                                       DepartmentInfo& info) const {
    const std::string cleanedName = trim(departmentName);
    if (cleanedName.empty()) {
        return false;
    }

    const DepartmentNode* node = findNode(root.get(), cleanedName);
    if (node == nullptr) {
        return false;
    }

    info.name = node->name;
    info.parentName = node->parent == nullptr ? "None" : node->parent->name;
    info.directChildCount = node->children.size();
    info.depth = 0;

    for (const DepartmentNode* current = node->parent;
         current != nullptr;
         current = current->parent) {
        ++info.depth;
    }

    return true;
}

// Prints the entire hierarchy or a clear empty-tree message.
void DepartmentTree::displayHierarchy(std::ostream& output) const {
    if (root == nullptr) {
        output << "No departments are loaded.\n";
        return;
    }

    displayHierarchy(root.get(), 0, output);
}

// Prints the preorder sequence on one line without hierarchy indentation.
void DepartmentTree::displayPreorder(std::ostream& output) const {
    if (root == nullptr) {
        output << "No departments are loaded.\n";
        return;
    }

    bool firstName = true;
    displayPreorder(root.get(), firstName, output);
    output << '\n';
}

// Checks whether a root is present.
bool DepartmentTree::empty() const {
    return root == nullptr;
}

// Reports the current number of owned nodes.
std::size_t DepartmentTree::size() const {
    return departmentCount;
}

// Releases the root and all descendants and resets the count.
void DepartmentTree::clear() {
    root.reset();
    departmentCount = 0;
}

// Removes leading and trailing whitespace, including tabs and carriage returns.
std::string DepartmentTree::trim(const std::string& text) {
    std::size_t first = 0;
    while (first < text.size()
           && std::isspace(static_cast<unsigned char>(text[first])) != 0) {
        ++first;
    }

    std::size_t last = text.size();
    while (last > first
           && std::isspace(static_cast<unsigned char>(text[last - 1])) != 0) {
        --last;
    }

    return text.substr(first, last - first);
}

// Compares two strings using lowercase ASCII character values.
bool DepartmentTree::equalsIgnoreCase(const std::string& left,
                                      const std::string& right) {
    if (left.size() != right.size()) {
        return false;
    }

    for (std::size_t index = 0; index < left.size(); ++index) {
        const unsigned char leftCharacter =
            static_cast<unsigned char>(left[index]);
        const unsigned char rightCharacter =
            static_cast<unsigned char>(right[index]);

        if (std::tolower(leftCharacter) != std::tolower(rightCharacter)) {
            return false;
        }
    }

    return true;
}

// Splits a record into all fields, including empty fields at either end.
std::vector<std::string> DepartmentTree::split(const std::string& line,
                                               char delimiter) {
    std::vector<std::string> fields;
    std::size_t fieldStart = 0;

    for (std::size_t index = 0; index <= line.size(); ++index) {
        if (index == line.size() || line[index] == delimiter) {
            fields.push_back(line.substr(fieldStart, index - fieldStart));
            fieldStart = index + 1;
        }
    }

    return fields;
}

// Extracts a portable display name from Unix or Windows-style paths.
std::string DepartmentTree::filenameOnly(const std::string& path) {
    const std::size_t separator = path.find_last_of("/\\");
    return separator == std::string::npos ? path : path.substr(separator + 1);
}

// Performs a depth-first search while allowing a matching node to be modified.
DepartmentTree::DepartmentNode* DepartmentTree::findNode(
    DepartmentNode* node,
    const std::string& departmentName) {
    if (node == nullptr) {
        return nullptr;
    }

    if (equalsIgnoreCase(node->name, departmentName)) {
        return node;
    }

    for (const std::unique_ptr<DepartmentNode>& child : node->children) {
        DepartmentNode* match = findNode(child.get(), departmentName);
        if (match != nullptr) {
            return match;
        }
    }

    return nullptr;
}

// Performs a read-only depth-first search for a case-insensitive name match.
const DepartmentTree::DepartmentNode* DepartmentTree::findNode(
    const DepartmentNode* node,
    const std::string& departmentName) {
    if (node == nullptr) {
        return nullptr;
    }

    if (equalsIgnoreCase(node->name, departmentName)) {
        return node;
    }

    for (const std::unique_ptr<DepartmentNode>& child : node->children) {
        const DepartmentNode* match = findNode(child.get(), departmentName);
        if (match != nullptr) {
            return match;
        }
    }

    return nullptr;
}

// Visits a node before its children and prints four spaces for each level.
void DepartmentTree::displayHierarchy(const DepartmentNode* node,
                                      std::size_t depth,
                                      std::ostream& output) {
    output << std::string(depth * 4, ' ') << node->name << '\n';

    for (const std::unique_ptr<DepartmentNode>& child : node->children) {
        displayHierarchy(child.get(), depth + 1, output);
    }
}

// Visits a node before its children and separates sequence entries with arrows.
void DepartmentTree::displayPreorder(const DepartmentNode* node,
                                     bool& firstName,
                                     std::ostream& output) {
    if (!firstName) {
        output << " -> ";
    }

    output << node->name;
    firstName = false;

    for (const std::unique_ptr<DepartmentNode>& child : node->children) {
        displayPreorder(child.get(), firstName, output);
    }
}
