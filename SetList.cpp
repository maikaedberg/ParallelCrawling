#include <mutex>
#include <string>
#include <iostream>

class NodeSetList {
public:
    std::mutex lock;
    std::string item;
    size_t key;
    NodeSetList * next;
    NodeSetList() {}
    NodeSetList(const std::string& s) {
        this->item = s;
        this->key = std::hash<std::string>{}(s);
        this->next = NULL;
    }
    NodeSetList(size_t k) {
        this->item = "";
        this->key = k;
        this->next = NULL;
    }
};

void DeleteNodeChain(NodeSetList* start) {
    NodeSetList* prev = start;
    NodeSetList* cur = prev->next;
    while (cur != NULL) {
        delete prev;
        prev = cur;
        cur = cur->next;
    }
    delete prev;
}

class SetList {
protected:
    NodeSetList* head;
    static const size_t LOWEST_KEY = 0;
    static const size_t LARGEST_KEY = ULONG_MAX;
    // returns the pointer to the last node with key < hash(val)
    // with keeping this and the next nodes locked
    NodeSetList* search(const std::string& val) const;
    std::mutex count_lock;
    int bound;
public:
    int count = 0;
    SetList(int max_size) {
        this->head = new NodeSetList(SetList::LOWEST_KEY);
        this->head->next = new NodeSetList(SetList::LARGEST_KEY);
        this->bound = max_size;
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
    NodeSetList* cur = this->head->next;
    while (cur->next != NULL) {
        ++result;
        cur = cur->next;
    }
    return result;
}

SetList::~SetList() {
    DeleteNodeChain(this->head);
}

NodeSetList* SetList::search(const std::string& val) const {
    NodeSetList *pred, *curr;
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
    NodeSetList* pred = this->search(val);

    NodeSetList* curr = pred->next;
    bool exists = (curr->key == std::hash<std::string>{}(val));
    count_lock.lock();
    if ( !exists && count < bound) {
        NodeSetList* node = new NodeSetList(val);
        node->next = curr;
        pred->next = node;
        count++;
    }
    count_lock.unlock();
    pred->lock.unlock();
    curr->lock.unlock();
    return !exists;
}

bool SetList::remove(const std::string& val) {
    NodeSetList* pred = this->search(val);
    NodeSetList* curr = pred->next;
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
    NodeSetList* pred = this->search(val);
    NodeSetList* curr = pred->next;
    bool exists = (curr->key == std::hash<std::string>{}(val));
    pred->lock.unlock();
    curr->lock.unlock();
    return exists;
}

void SetList::print() const {
    NodeSetList* cur = this->head->next;
    while (cur->next != NULL) {
        std::cout << cur->item << "\n";

        cur = cur->next;
    }
    std::cout << std::endl;
}