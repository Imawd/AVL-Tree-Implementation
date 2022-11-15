#ifndef RBBST_H
#define RBBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor and setting
* the color to red since every new node will be red when it is first inserted.
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here

    //p is parent and n is newly-inserted node
    virtual void insertFix(AVLNode<Key,Value>* p, AVLNode<Key,Value>* n);
		virtual void removeFix(AVLNode<Key,Value>* n, int diff);
    virtual void rotateRight(AVLNode<Key,Value>* node);
    virtual void rotateLeft(AVLNode<Key,Value>* node);
    bool zigZig(AVLNode<Key,Value>* g, AVLNode<Key,Value>* p,AVLNode<Key,Value>* n);
    bool zigZag(AVLNode<Key,Value>* g, AVLNode<Key,Value>* p,AVLNode<Key,Value>* n);


};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // TODO
    if (this->empty()) {
        AVLNode<Key, Value>* nodeToAdd = new AVLNode<Key, Value>(new_item.first, new_item.second, NULL);
				nodeToAdd->setBalance(0);
        this->root_ = nodeToAdd;
    }

    else if (this->internalFind(new_item.first) != NULL) {
        AVLNode<Key, Value>* toChange = static_cast<AVLNode<Key, Value>*>(this->internalFind(new_item.first));
        toChange->setValue(new_item.second); //only change value if key exists
    }

    else {
        AVLNode<Key, Value>* temp = static_cast<AVLNode<Key, Value>*>(this->root_);
        AVLNode<Key, Value>* p = NULL;
        while (temp != NULL) {
            if (temp->getKey() > new_item.first) {
                p = temp;
                temp = temp->getLeft();
            }
            else if (temp->getKey() < new_item.first) {
                p = temp;
                temp = temp->getRight();
            }
        }
				int diff = 0;  
        AVLNode<Key, Value>* nodeToAdd = new AVLNode<Key, Value>(new_item.first, new_item.second, p);
				nodeToAdd->setBalance(0);
        if (p->getKey() > new_item.first) {
            p->setLeft(nodeToAdd);
						diff = -1;
        }
        else {
            p->setRight(nodeToAdd);
						diff = 1;
        }

				//update balances and insertFix if necessary
				if (p->getBalance() == 1 || p->getBalance() == -1) p->setBalance(0);
				else if(p->getBalance() == 0) {
						p->setBalance(p->getBalance() + diff);
						insertFix(p, nodeToAdd);
				}
    }
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::remove(const Key& key)
{
	// TODO

	AVLNode<Key, Value>* toRemove = static_cast<AVLNode<Key, Value>*>(this->internalFind(key));
	
	if (toRemove == NULL) return;

	//case for if there are 2 children
	if (toRemove->getLeft() != NULL && toRemove->getRight() != NULL) {
		AVLNode<Key, Value>* pred = static_cast<AVLNode<Key, Value>*>(this->predecessor(toRemove));
		nodeSwap(toRemove, pred);
	}

	//now it is guaranteed that there will only be one child or zero children
	AVLNode<Key, Value>* p = toRemove->getParent();
	int diff = 0;
	if (p != NULL) {
		//case for if n is a left child
		if (p->getLeft() == toRemove) {
			diff = 1;
		}

		//case for if n is a right child
		else if (p->getRight() == toRemove) {
			diff = -1;
		}
	}

	//node has 0 children
	if (toRemove->getRight() == NULL && toRemove->getLeft() == NULL) {
			//first check if toRemove is root
			if (p == NULL) {
				delete toRemove;
				this->root_ = NULL;
				return;
			}

			//check if toRemove is left node
			else if (p->getLeft() == toRemove) {
				//if toRemove is a left child then set left child of parent to NULL
				p->setLeft(NULL);
				delete toRemove;
			}

			//check if toRemove is a right node
			else if (p->getRight() == toRemove) {
				//if its a right node, set right node to NULL
				p->setRight(NULL);
				delete toRemove;
			}
	}

	//remaining cases where there is only one child node
	else {
		//if toRemove is a root:
		if (p == NULL) {
			//if has a left child
			if (toRemove->getRight() == NULL) {
				toRemove->getLeft()->setParent(NULL);
				this->root_ = toRemove->getLeft();
				delete toRemove;
			}
			//if has a right child
			else if (toRemove->getLeft() == NULL) {
				toRemove->getRight()->setParent(NULL);
				this->root_ = toRemove->getRight();
				delete toRemove;
			}
		}

		//non-root cases:
		//
		//if has a right child:
		else if (toRemove->getLeft() == NULL) {
			//if toRemove itself is a left node
			if (p->getLeft() == toRemove) {
				//promote child
				p->setLeft(toRemove->getRight());
			}
			//if toRemove itself is a right node
			else {
				//promote child
				p->setRight(toRemove->getRight());
			}
			toRemove->getRight()->setParent(p);
			delete toRemove;
		}
		//
		//if has a left child
		else if (toRemove->getRight() == NULL) {
			//if toRemove itself is a left node
			if (p->getLeft() == toRemove) {
				p->setLeft(toRemove->getLeft());
			}
			else {
				p->setRight(toRemove->getLeft());
			}
			toRemove->getLeft()->setParent(p);
			delete toRemove;
		}
	}
	removeFix(p, diff);
}


template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}

template<class Key, class Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key,Value>* p, AVLNode<Key,Value>* n) {
    if (p == NULL || p->getParent() == NULL) return;
    //define grandparent g
    AVLNode<Key, Value>* g = p->getParent();

    //if parent is left child of grandparent
    if (p == g->getLeft()) {
        g->setBalance(g->getBalance() - 1);
        if (g->getBalance() == 0) return;
        else if(g->getBalance() == -1) {
            insertFix(g, p);
        }

        else if (g->getBalance() == -2) {
            if (zigZig(g, p, n)) {
                rotateRight(g);
                g->setBalance(0);
                p->setBalance(0);
            }

            else if (zigZag(g, p, n)) {
                rotateLeft(p);
                rotateRight(g);
                if (n->getBalance() == -1) {
                    g->setBalance(1);
                    p->setBalance(0);
                    n->setBalance(0);
                }
                else if (n->getBalance() == 0) {
                    g->setBalance(0);
                    p->setBalance(0);
                    n->setBalance(0);                    
                }

                else if (n->getBalance() == 1) {
                    g->setBalance(0);
                    p->setBalance(-1);
                    n->setBalance(0);                    
                }
            }
        }
    }

    else if (p == g->getRight()) {
        g->setBalance(g->getBalance() + 1);

        if (g->getBalance() == 0) return;

        else if(g->getBalance() == 1) {
            insertFix(g, p);
        }

        else if (g->getBalance() == 2) {
            if (zigZig(g, p, n)) {
                rotateLeft(g);
                g->setBalance(0);
                p->setBalance(0);
            }

            else if (zigZag(g, p, n)) {
                rotateRight(p);
                rotateLeft(g);
                if (n->getBalance() == 1) {
                    g->setBalance(-1);
                    p->setBalance(0);
                    n->setBalance(0);
                }
                else if (n->getBalance() == 0) {
                    g->setBalance(0);
                    p->setBalance(0);
                    n->setBalance(0);                    
                }

                else if (n->getBalance() == -1) {
                    g->setBalance(0);
                    p->setBalance(1);
                    n->setBalance(0);                    
                }
            }
        }
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key,Value>* n, int diff) {
	if (n == NULL) return;

	AVLNode<Key, Value>* p = n->getParent();

	int nextdiff = 0;

	//determine next diff based on wehether n is a left or right node
	if (p != NULL) {
		if (p->getLeft() == n) {
			nextdiff = 1;
		}

		else {
			nextdiff = -1;
		}
	}

	//unbalanced at grandparent towards left
	if (n->getBalance() + diff == -2) {
		AVLNode<Key, Value>* c = n->getLeft();
		if (c == NULL) return; //c should NOT ever be NULL but if it is this should prevent segfault
		if (c->getBalance() == -1) {
			rotateRight(n);
			n->setBalance(0);
			c->setBalance(0);
			removeFix(p, nextdiff);
		}

		else if (c->getBalance() == 0) {
			rotateRight(n);
			n->setBalance(-1);
			c->setBalance(1);
			return;
		}

		else if (c->getBalance() == 1) {
			AVLNode<Key, Value>* g = c->getRight();
			if (g == NULL) return; //g should also never be NULL but in the event it is, this line prevents a segfault
			rotateLeft(c);
			rotateRight(n);
			if (g->getBalance() == 1) {
				n->setBalance(0);
				c->setBalance(-1);
				g->setBalance(0);
			}

			else if (g->getBalance() == 0) {
				n->setBalance(0);
				c->setBalance(0);
				g->setBalance(0);
			}

			else if (g->getBalance() == -1) {
				n->setBalance(1);
				c->setBalance(0);
				g->setBalance(0);
			}
			removeFix(p, nextdiff);
		}
	}

	//unbalanced at grandparent towards right
	else if (n->getBalance() + diff == 2) {
		AVLNode<Key, Value>* c = n->getRight();
		if (c == NULL) return;
		if (c->getBalance() == 1) {
			rotateLeft(n);
			n->setBalance(0);
			c->setBalance(0);
			removeFix(p, nextdiff);
		}

		else if (c->getBalance() == 0) {
			rotateLeft(n);
			n->setBalance(1);
			c->setBalance(-1);
			return;
		}

		else if (c->getBalance() == -1) {
			AVLNode<Key, Value>* g = c->getLeft();
			if (g == NULL) return;
			rotateRight(c);
			rotateLeft(n);
			if (g->getBalance() == -1) {
				n->setBalance(0);
				c->setBalance(1);
				g->setBalance(0);
			}

			else if (g->getBalance() == 0) {
				n->setBalance(0);
				c->setBalance(0);
				g->setBalance(0);
			}

			else if (g->getBalance() == 1) {
				n->setBalance(-1);
				c->setBalance(0);
				g->setBalance(0);
			}
			removeFix(p, nextdiff);
		}
	}
	else if (n->getBalance() + diff == 1) {
		n->setBalance(1);
		return;
	}

	else if (n->getBalance() + diff == -1) {
		n->setBalance(-1);
		return;
	}

	else {
		n->setBalance(0);
		removeFix(p, nextdiff);
	}
}


//helper to rotate right
template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key,Value>* node) {

	AVLNode<Key,Value>* temp = NULL;
	AVLNode<Key, Value>* parent = node->getParent();
	AVLNode<Key, Value>* left = node->getLeft();
		//if the left child of node has a right  child node, it needs to be tracked using a temp so that we can access it later
		//if we don't do this before setting nodes, we will lose this potential node
		if (node->getLeft()->getRight() != NULL) {
				temp = left->getRight();
				left->setRight(NULL);
		}
		// Rotating - root case
		if (parent == NULL) {
				this->root_ = left;
				left->setParent(NULL);
				node->setParent(left);
				left->setRight(node);
		}
		//Rotating - non-root case
		else {
			//check if node is a left child
			if (parent->getLeft() == node) {
				parent->setLeft(node->getLeft());
			}
			//check if node is a right child
			else {
				parent->setRight(node->getLeft());
			}
			left->setParent(node->getParent());
			node->setParent(left);
			left->setRight(node);
		}

		//check if the afformentioned right node of left child exists 
		if (temp != NULL) {
			//set left child to be temp and temp's parent to be node after rotation is done
			//this is to prevent losing nodes 
				node->setLeft(temp);
				temp->setParent(node);
		}
		else {
				node->setLeft(NULL);
		}
}
    
//helper to rotate left
template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key,Value>* node) {

	AVLNode<Key,Value>* temp = NULL;
	AVLNode<Key, Value>* parent = node->getParent();
	AVLNode<Key, Value>* right = node->getRight();
    //track left child of right child node which will otherwise be lost if it exists
    if (right->getLeft() != NULL) {
        temp = right->getLeft();
        right->setLeft(NULL);
    }
    //Rotating - root case
    if (parent == NULL) {
        this->root_ = right;
        right->setParent(NULL);
        node->setParent(node->getRight());
        right->setLeft(node);
    }
		//Rotating - non-root case
    else {
			//if node is a left child
        if (parent->getLeft() == node) {
            parent->setLeft(right);
        }
				//if node is a right child
        else {
            parent->setRight(right);
        }
        right->setParent(node->getParent());
        node->setParent(right);
        right->setLeft(node);
    }
    // Check for orphan
    if (temp != NULL) {
        node->setRight(temp);
        temp->setParent(node);
    }
    else {
        node->setRight(NULL);
    }
		
}

//helper that determines zig-zig cases
template<class Key, class Value>
bool AVLTree<Key, Value>::zigZig(AVLNode<Key,Value>* g, AVLNode<Key,Value>* p,AVLNode<Key,Value>* n) {
    //returns true if child and grandchild are both left or both right otherwise returns false
    if ((g->getLeft() == p && p->getLeft() == n) || (g->getRight() == p && p->getRight() == n)) return true;
    else return false;
}

//helper that determines zip-zag cases
template<class Key, class Value>
bool AVLTree<Key, Value>::zigZag(AVLNode<Key,Value>* g, AVLNode<Key,Value>* p,AVLNode<Key,Value>* n) {
	//returns true if child and grandchild are a left-right or right-left combo
    if ((g->getLeft() == p && p->getRight() == n) || (g->getRight() == p && p->getLeft() == n)) return true;
    else return false;
}
#endif
