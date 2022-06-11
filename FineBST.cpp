#include <algorithm>
#include <atomic>
#include <climits>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
#include <stdexcept>


//-----------------------------------------------------------------------------

class Node {
public:
    long key;
    std::mutex lock;
    Node* left;
    Node* right;
    Node* parent;
    Node() {}
    Node(long k) {
        this->key = k;
        this->left = NULL;
        this->right = NULL;
        this->parent = NULL;
    }
};

class FineBST {
protected:
    Node* root;
    static const unsigned long LOWEST_KEY = LONG_MIN;
    // returns the node that contains k or a node that would be
    // a parent of k if it will be inserted
    // Note: after the execution the returned node and its parent should be locked
    static Node* search(Node* root, long k);
    // removes the given node. Works under the assumption that the node and its parent
    // are locked
    static void remove_node(Node* n);
    std::mutex count_lock;
public:
    int count = 0;
    FineBST() {
        this->root = new Node(FineBST::LOWEST_KEY);
    }
    ~FineBST();
    bool add(long k);
    bool remove(long k);
    bool contains(long k);
    void print();
};

void DeleteTree(Node* root) {
    std::lock_guard<std::mutex> lk(root->lock);
    if (root->left != NULL) {
        DeleteTree(root->left);
    }
    if (root->right != NULL) {
        DeleteTree(root->right);
    }
    delete root;
}

FineBST::~FineBST() {
    DeleteTree(this->root);
}

Node* FineBST::search(Node* root, long k) {
    Node* cur = root;
    cur->lock.lock();
    if (k > cur->key && cur->right != NULL) {
        if (cur->parent != NULL) { cur->parent->lock.unlock(); }
        return search(cur->right, k);
    }
    if (k < cur->key && cur->left != NULL) {
        if (cur->parent != NULL) { cur->parent->lock.unlock(); }
        return search(cur->left, k);
    }
    return cur;
}

bool FineBST::contains(long k) {
    Node* node = FineBST::search(this->root, k);
    if ( node->parent ){ node->parent->lock.unlock();}
    bool exists = (node->key == k);
    node->lock.unlock();
    return exists;
}

bool FineBST::add(long k) {

    Node* curr = FineBST::search(this->root, k);

    if (curr->parent){
        curr->parent->lock.unlock();
    }

    bool exists = (curr->key == k);

    if (! exists){
        count_lock.
        Node* node = new Node(k);
        node->parent = curr;
        if (k < curr->key){
            curr->left = node;
        }
        else{
            curr->right = node;
        }
    }
    curr->lock.unlock();

    return !exists;
}
