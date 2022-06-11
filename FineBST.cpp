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
    std::string str;
    std::mutex lock;
    NodeBST* left;
    NodeBST* right;
    NodeBST* parent;
    NodeBST(long k) {
        this->str = "";
        this->left = NULL;
        this->right = NULL;
        this->parent = NULL;
        this->key = k;
    }
    NodeBST(std::string link) {
        this->str = link;
        this->left = NULL;
        this->right = NULL;
        this->parent = NULL;
        this->key = std::hash<std::string>{}(link);
    }
    void print();
};

void NodeBST::print(){
    std::cout << this->str << '\n';
    if (this->left)
        this->left->print();
    if (this->right)
        this->right->print();
}

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
    int bound;
public:
    int count = 0;
    FineBST(int max_size) {
        this->root = new NodeBST(FineBST::LOWEST_KEY);
        this->bound = max_size;
    }
    ~FineBST();
    bool add(std::string link);
    void print(){ root->print(); };
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

bool FineBST::add(std::string link) {
    long k = std::hash<std::string>{}(link);
    NodeBST* curr = FineBST::search(this->root, k);

    if (curr->parent){
        curr->parent->lock.unlock();
    }

    bool exists = (curr->key == k);

    count_lock.lock();
    if ( ! exists && count < bound ){
        NodeBST* node = new NodeBST(link);
        node->parent = curr;
        if (k < curr->key){
            curr->left = node;
        }
        else{
            curr->right = node;
        }
        count++;
    }
    count_lock.unlock();
    curr->lock.unlock();
    

    return (! exists && count < bound) ;
}