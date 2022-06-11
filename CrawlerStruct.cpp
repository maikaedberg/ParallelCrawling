
#include <string>
#include <iostream>
#include <regex>
#include <curl/curl.h>
#include "FineBST.cpp"
#include "SetList.cpp"
#include "SafeUnboundedQueue.cpp"

class HtmlLink {
public:
    std::string url;
    //std::string firstlink;
    bool isempty() { return url == ""; }
    HtmlLink() {
        this->url = "";
    }
    HtmlLink(std::string link) {
        this->url = link;
    }
    HtmlLink(HtmlLink firstLink, std::string href) {
        std::string link = ( href ).substr(9, href.size() - 10);
        if ( link.find("https://")  == std::string::npos ){
            if ( ( link.substr(0,2) == "//" ) )
                this->url = "https:" + url;
            else
                this->url = firstLink.url + link;
        }
    }
    
};

template <typename T>
class CrawlerStruct {
protected:
    T* LinkDirectory;
    SafeUnboundedQueue<HtmlLink>* links;
    HtmlLink firstLink;
    int num_threads;
    int bound;

public:
    CrawlerStruct( HtmlLink firstLink, int no_threads, int bound) {
        this->firstLink = firstLink;
        this->bound = bound;
        this->LinkDirectory = new T(bound);
        this->links = new SafeUnboundedQueue<HtmlLink>;
        this->LinkDirectory->add(firstLink.url);
        this->links->push(firstLink.url);
    }
    void print() { LinkDirectory->print(); }
    HtmlLink pop(){ return this->links->pop(); }

    bool add( HtmlLink linkstr){
        if ( this->LinkDirectory->add(linkstr.url) ) {
            this->links->push(linkstr.url);
            return true;
        }
        return false;
    }

    bool isFull() { return LinkDirectory->count == this->bound; }
    size_t size() { return LinkDirectory->count; }
    void decrementLinks() { links->decrementLinks(); }

};


