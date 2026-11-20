#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here
bool checkPaths(Node* node, int currentDepth, int& targetDepth);


bool equalPaths(Node * root)
{
    // Add your code below
    if (root == nullptr) {
        return true;
    }
    int targetDepth = -1;
    return checkPaths(root, 0, targetDepth);
}

bool checkPaths(Node* node, int currentDepth, int& targetDepth)
{
    // Base case: reached a leaf node
    if (node->left == nullptr && node->right == nullptr) {
        if (targetDepth == -1) {
            targetDepth = currentDepth;
            return true;
        }
        return currentDepth == targetDepth;
    }

    // Only recurse down existing children (a nullptr child is not a leaf)
    bool leftEqual = true;
    bool rightEqual = true;

    if (node->left != nullptr) {
        leftEqual = checkPaths(node->left, currentDepth + 1, targetDepth);
    }
    if (node->right != nullptr) {
        rightEqual = checkPaths(node->right, currentDepth + 1, targetDepth);
    }

    return leftEqual && rightEqual;
}