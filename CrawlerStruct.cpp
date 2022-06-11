
#include <string>
#include <iostream>
#include "FineBST.cpp"
#include "SetList.cpp"
#include "SafeUnboundedQueue.cpp"

class HtmlLink {
public:
    std::string url;
    std::string firstlink;
    bool isempty() { return url == ""; }
    HtmlLink() {
        this->firstlink = "";
        this->url = "";
    }
    HtmlLink(std::string link) {
        this->firstlink = "";
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
    T LinkDirectory;
    SafeUnboundedQueue<HtmlLink> links;
    std::string firstLink;
    int num_threads;
    int bound;

public:
    CrawlerStruct( std::string firstLink, int num_threads, int bound) {
        this->firstLink = firstLink;
        this->num_threads = num_threads;
        this->bound = bound;
        this->LinkDirectory = T(bound);
        this->LinkDirectory.add(firstLink);
        this->links.push(firstLink);
    }
    void print() { LinkDirectory.print(); }

};