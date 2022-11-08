#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here
int calculateHeight(Node* root);
int max(int x, int y); //not sure if allowed #include <algorithm>
bool equalPaths(Node * root)
{
    // Add your code below
    if (root == NULL) return true;

    if (root->left == NULL && root->right == NULL) return true;

    if (root->left == NULL) return calculateHeight(root->right) == 1;

    if (root->right == NULL) return calculateHeight(root->left) == 1;

    return ((calculateHeight(root->right) == calculateHeight(root->left)) && equalPaths(root->left) && equalPaths(root->right));
    
}

int calculateHeight(Node* root) {
    if (root == NULL) return 0;
    return (1 + max(calculateHeight(root->left), calculateHeight(root->right)));
}

int max(int x, int y) {
    if (x > y) return x;
    else if (y > x) return y;
    else return x;
}


