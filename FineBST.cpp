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

class NodeBST {
public:
    long key;
    std::mutex lock;
    NodeBST* left;
    NodeBST* right;
    NodeBST* parent;
    NodeBST() {}
    NodeBST(long k) {
        this->key = k;
        this->left = NULL;
        this->right = NULL;
        this->parent = NULL;
    }
};

class FineBST {
protected:
    NodeBST* root;
    static const unsigned long LOWEST_KEY = LONG_MIN;
    // returns the node that contains k or a node that would be
    // a parent of k if it will be inserted
    // Note: after the execution the returned node and its parent should be locked
    static NodeBST* search(NodeBST* root, long k);
    // removes the given node. Works under the assumption that the node and its parent
    // are locked
    static void remove_node(NodeBST* n);
    std::mutex count_lock;
public:
    int count = 0;
    FineBST() {
        this->root = new NodeBST(FineBST::LOWEST_KEY);
    }
    ~FineBST();
    bool add(long k);
    bool remove(long k);
    bool contains(long k);
    void print();
};

void DeleteTree(NodeBST* root) {
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

NodeBST* FineBST::search(NodeBST* root, long k) {
    NodeBST* cur = root;
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
    NodeBST* node = FineBST::search(this->root, k);
    if ( node->parent ){ node->parent->lock.unlock();}
    bool exists = (node->key == k);
    node->lock.unlock();
    return exists;
}

bool FineBST::add(long k) {

    NodeBST* curr = FineBST::search(this->root, k);

    if (curr->parent){
        curr->parent->lock.unlock();
    }

    bool exists = (curr->key == k);

    if (! exists){
        NodeBST* node = new NodeBST(k);
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
