#ifndef AVLBST_H
#define AVLBST_H

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
* An explicit constructor to initialize the elements by calling the base class constructor
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
    void rotateLeft(AVLNode<Key, Value>* x);
    void rotateRight(AVLNode<Key, Value>* x);
    void insertFix(AVLNode<Key, Value>* parent, AVLNode<Key, Value>* node);
    void removeFix(AVLNode<Key, Value>* node, int8_t diff);
};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    if (this->root_ == NULL) {
        this->root_ = new AVLNode<Key, Value>(new_item.first, new_item.second, NULL);
        return;
    }

    AVLNode<Key, Value>* curr = static_cast<AVLNode<Key, Value>*>(this->root_);
    AVLNode<Key, Value>* parent = NULL;

    while (curr != NULL) {
        parent = curr;
        if (new_item.first < curr->getKey()) {
            curr = curr->getLeft();
        }
        else if (curr->getKey() < new_item.first) {
            curr = curr->getRight();
        }
        else {
            curr->setValue(new_item.second);
            return;
        }
    }

    AVLNode<Key, Value>* newNode = new AVLNode<Key, Value>(new_item.first, new_item.second, parent);
    if (new_item.first < parent->getKey()) {
        parent->setLeft(newNode);
        if (parent->getBalance() == 1) {
            parent->setBalance(0);
        }
        else {
            parent->setBalance(-1);
            insertFix(parent, newNode);
        }
    }
    else {
        parent->setRight(newNode);
        if (parent->getBalance() == -1) {
            parent->setBalance(0);
        }
        else {
            parent->setBalance(1);
            insertFix(parent, newNode);
        }
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key, Value>* parent, AVLNode<Key, Value>* node)
{
    if (parent == NULL || parent->getParent() == NULL) {
        return;
    }

    AVLNode<Key, Value>* g = parent->getParent();

    if (parent == g->getLeft()) {
        g->updateBalance(-1);
        int8_t gBal = g->getBalance();

        if (gBal == 0) {
            return;
        }
        else if (gBal == -1) {
            insertFix(g, parent);
        }
        else if (gBal == -2) {
            if (node == parent->getLeft()) {
                // Zig-zig (Left-Left)
                rotateRight(g);
                parent->setBalance(0);
                g->setBalance(0);
            }
            else {
                // Zig-zag (Left-Right)
                rotateLeft(parent);
                rotateRight(g);
                if (node->getBalance() == -1) {
                    parent->setBalance(0);
                    g->setBalance(1);
                }
                else if (node->getBalance() == 0) {
                    parent->setBalance(0);
                    g->setBalance(0);
                }
                else {
                    parent->setBalance(-1);
                    g->setBalance(0);
                }
                node->setBalance(0);
            }
        }
    }
    else {
        g->updateBalance(1);
        int8_t gBal = g->getBalance();

        if (gBal == 0) {
            return;
        }
        else if (gBal == 1) {
            insertFix(g, parent);
        }
        else if (gBal == 2) {
            if (node == parent->getRight()) {
                // Zig-zig (Right-Right)
                rotateLeft(g);
                parent->setBalance(0);
                g->setBalance(0);
            }
            else {
                // Zig-zag (Right-Left)
                rotateRight(parent);
                rotateLeft(g);
                if (node->getBalance() == 1) {
                    parent->setBalance(0);
                    g->setBalance(-1);
                }
                else if (node->getBalance() == 0) {
                    parent->setBalance(0);
                    g->setBalance(0);
                }
                else {
                    parent->setBalance(1);
                    g->setBalance(0);
                }
                node->setBalance(0);
            }
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
    AVLNode<Key, Value>* curr = static_cast<AVLNode<Key, Value>*>(this->internalFind(key));
    if (curr == NULL) {
        return;
    }

    // If curr has 2 children, swap with its predecessor
    if (curr->getLeft() != NULL && curr->getRight() != NULL) {
        AVLNode<Key, Value>* pred = static_cast<AVLNode<Key, Value>*>(BinarySearchTree<Key, Value>::predecessor(curr));
        nodeSwap(curr, pred);
    }

    AVLNode<Key, Value>* parent = curr->getParent();
    int8_t diff = 0;
    if (parent != NULL) {
        if (parent->getLeft() == curr) {
            diff = 1;
        }
        else {
            diff = -1;
        }
    }

    // curr now has at most 1 child
    AVLNode<Key, Value>* child = (curr->getLeft() != NULL) ? curr->getLeft() : curr->getRight();

    if (child != NULL) {
        child->setParent(parent);
    }

    if (parent == NULL) {
        this->root_ = child;
    }
    else if (parent->getLeft() == curr) {
        parent->setLeft(child);
    }
    else {
        parent->setRight(child);
    }

    delete curr;

    removeFix(parent, diff);
}

template<class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key, Value>* node, int8_t diff)
{
    if (node == NULL) {
        return;
    }

    AVLNode<Key, Value>* parent = node->getParent();
    int8_t nextDiff = 0;
    if (parent != NULL) {
        if (parent->getLeft() == node) {
            nextDiff = 1;
        }
        else {
            nextDiff = -1;
        }
    }

    int8_t newBal = node->getBalance() + diff;

    if (newBal == -2) {
        AVLNode<Key, Value>* c = node->getLeft();
        int8_t cBal = c->getBalance();

        if (cBal == -1) {
            rotateRight(node);
            node->setBalance(0);
            c->setBalance(0);
            removeFix(parent, nextDiff);
        }
        else if (cBal == 0) {
            rotateRight(node);
            node->setBalance(-1);
            c->setBalance(1);
        }
        else {
            AVLNode<Key, Value>* g = c->getRight();
            int8_t gBal = g->getBalance();
            rotateLeft(c);
            rotateRight(node);

            if (gBal == 1) {
                node->setBalance(0);
                c->setBalance(-1);
            }
            else if (gBal == 0) {
                node->setBalance(0);
                c->setBalance(0);
            }
            else {
                node->setBalance(1);
                c->setBalance(0);
            }
            g->setBalance(0);
            removeFix(parent, nextDiff);
        }
    }
    else if (newBal == 2) {
        AVLNode<Key, Value>* c = node->getRight();
        int8_t cBal = c->getBalance();

        if (cBal == 1) {
            rotateLeft(node);
            node->setBalance(0);
            c->setBalance(0);
            removeFix(parent, nextDiff);
        }
        else if (cBal == 0) {
            rotateLeft(node);
            node->setBalance(1);
            c->setBalance(-1);
        }
        else {
            AVLNode<Key, Value>* g = c->getLeft();
            int8_t gBal = g->getBalance();
            rotateRight(c);
            rotateLeft(node);

            if (gBal == -1) {
                node->setBalance(0);
                c->setBalance(1);
            }
            else if (gBal == 0) {
                node->setBalance(0);
                c->setBalance(0);
            }
            else {
                node->setBalance(-1);
                c->setBalance(0);
            }
            g->setBalance(0);
            removeFix(parent, nextDiff);
        }
    }
    else if (newBal == -1 || newBal == 1) {
        node->setBalance(newBal);
    }
    else if (newBal == 0) {
        node->setBalance(0);
        removeFix(parent, nextDiff);
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* x)
{
    if (x == NULL || x->getRight() == NULL) {
        return;
    }

    AVLNode<Key, Value>* y = x->getRight();
    AVLNode<Key, Value>* p = x->getParent();
    AVLNode<Key, Value>* b = y->getLeft();

    y->setParent(p);
    if (p == NULL) {
        this->root_ = y;
    }
    else if (p->getLeft() == x) {
        p->setLeft(y);
    }
    else {
        p->setRight(y);
    }

    y->setLeft(x);
    x->setParent(y);

    x->setRight(b);
    if (b != NULL) {
        b->setParent(x);
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* x)
{
    if (x == NULL || x->getLeft() == NULL) {
        return;
    }

    AVLNode<Key, Value>* y = x->getLeft();
    AVLNode<Key, Value>* p = x->getParent();
    AVLNode<Key, Value>* b = y->getRight();

    y->setParent(p);
    if (p == NULL) {
        this->root_ = y;
    }
    else if (p->getLeft() == x) {
        p->setLeft(y);
    }
    else {
        p->setRight(y);
    }

    y->setRight(x);
    x->setParent(y);

    x->setLeft(b);
    if (b != NULL) {
        b->setParent(x);
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}


#endif