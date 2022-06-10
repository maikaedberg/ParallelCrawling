#include <mutex>
#include <string>
#include <iostream>

class Node {
public:
    std::mutex lock;
    std::string item;
    size_t key;
    Node * next;
    Node() {}
    Node(const std::string& s) {
        this->item = s;
        this->key = std::hash<std::string>{}(s);
        this->next = NULL;
    }
    Node(size_t k) {
        this->item = "";
        this->key = k;
        this->next = NULL;
    }
};

void DeleteNodeChain(Node* start) {
    Node* prev = start;
    Node* cur = prev->next;
    while (cur != NULL) {
        delete prev;
        prev = cur;
        cur = cur->next;
    }
    delete prev;
}

class SetList {
protected:
    Node* head;
    static const size_t LOWEST_KEY = 0;
    static const size_t LARGEST_KEY = ULONG_MAX;
    // returns the pointer to the last node with key < hash(val)
    // with keeping this and the next nodes locked
    Node* search(const std::string& val) const;
    std::mutex count_lock;
public:
    int count = 0;
    SetList() {
        this->head = new Node(SetList::LOWEST_KEY);
        this->head->next = new Node(SetList::LARGEST_KEY);
    }
    ~SetList();
    bool add(const std::string& val);
    bool remove(const std::string& val);
    bool contains(const std::string& val) const;
    template <typename F>
    void transform(F f);
    void print() const; // for testing
    size_t size() const; // for testing
};

size_t SetList::size() const {
    size_t result = 0;
    Node* cur = this->head->next;
    while (cur->next != NULL) {
        ++result;
        cur = cur->next;
    }
    return result;
}

SetList::~SetList() {
    DeleteNodeChain(this->head);
}

Node* SetList::search(const std::string& val) const {
    Node *pred, *curr;
    size_t key = std::hash<std::string>{}(val);
    pred = head;
    pred->lock.lock();
    curr = pred->next;
    curr->lock.lock();
    while (curr->key < key) {
        pred->lock.unlock();
        pred = curr;
        curr = curr->next;
        curr->lock.lock();
    }
    return pred; 
}

bool SetList::add(const std::string& val) {
    Node* pred = this->search(val);

    Node* curr = pred->next;
    bool exists = (curr->key == std::hash<std::string>{}(val));
    if (!exists) {
        Node* node = new Node(val);
        node->next = curr;
        pred->next = node;
        count_lock.lock();
        count++;
        count_lock.unlock();
    }
    pred->lock.unlock();
    curr->lock.unlock();
    return !exists;
}

bool SetList::remove(const std::string& val) {
    Node* pred = this->search(val);
    Node* curr = pred->next;
    bool exists = (curr->key == std::hash<std::string>{}(val));
    curr->lock.unlock();
    if (exists) {
        pred->next = curr->next;
        delete curr;
    }
    pred->lock.unlock();
    return exists;
}

bool SetList::contains(const std::string& val) const {
    Node* pred = this->search(val);
    Node* curr = pred->next;
    bool exists = (curr->key == std::hash<std::string>{}(val));
    pred->lock.unlock();
    curr->lock.unlock();
    return exists;
}

void SetList::print() const {
    Node* cur = this->head->next;
    while (cur->next != NULL) {
        std::cout << cur->item << "\n";

        cur = cur->next;
    }
    std::cout << std::endl;
}

