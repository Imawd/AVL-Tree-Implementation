#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here
int calculateHeight(Node* root);

bool equalPaths(Node * root)
{
    // Add your code below
    if (root == nullptr) return false;
    else if (root->left == nullptr && root->right == nullptr) return true;
    else return (equalPaths(root->left) && equalPaths(root->right));
}


