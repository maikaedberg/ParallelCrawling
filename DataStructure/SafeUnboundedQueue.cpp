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
#include <cstring>


template <class E> 
class SafeUnboundedQueue {
        std::queue<E> elements;
        std::mutex lock;
        int count_links = 0;
        std::condition_variable not_empty_or_done;
        
    public: 
        SafeUnboundedQueue<E>(){}
        void push(const E& element);
        E pop ();
        bool is_empty() const {return this->elements.empty();}
        bool work_ended() const { return this->elements.empty() && count_links == 0; }
        void incrementLinks();
        void decrementLinks();
};

template <class E> 
void SafeUnboundedQueue<E>::push(const E& element) {
    std::unique_lock<std::mutex> lk(lock);
    bool wasEmpty = elements.empty();
    elements.push(element);
    if ( wasEmpty )
        not_empty_or_done.notify_all();
}

template <class E> 
E SafeUnboundedQueue<E>::pop() {
    std::unique_lock<std::mutex> lk(lock);
    while ( elements.empty() && count_links > 0 ) 
        not_empty_or_done.wait(lk);
    if ( elements.empty() )
        return E();
    E element = elements.front();

    elements.pop();
    count_links++;
    return element;
}

template <class E> 
void SafeUnboundedQueue<E>::decrementLinks() {
    std::unique_lock<std::mutex> lk(lock);
    count_links--;
    if ( elements.empty() and count_links == 0)
        not_empty_or_done.notify_all();
}