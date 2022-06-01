#include <algorithm>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <future>
#include <iostream>
#include <cmath>
#include <mutex>
#include <numeric>
#include <thread>
#include <queue>


template <class E> 
class SafeUnboundedQueue {
        std::queue<E> elements;
        std::mutex lock;
        std::condition_variable not_empty;
    public: 
        SafeUnboundedQueue<E>(){}
        void push(const E& element);
        E pop ();
        bool is_empty() const {return this->elements.empty();}
};

template <class E>
void SafeUnboundedQueue<E>::push(const E& element) {
    std::unique_lock<std::mutex> lk(lock);
    bool wasEmpty = elements.empty();
    elements.push(element);
    if (wasEmpty)
        not_empty.notify_all();
}

template <class E> 
E SafeUnboundedQueue<E>::pop() {
    std::unique_lock<std::mutex> lk(lock);
    while (elements.empty())
        not_empty.wait(lk);
    E element = elements.front();
    elements.pop();
    return element;
}


