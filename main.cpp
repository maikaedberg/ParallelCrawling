#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <algorithm>

#include <curl/curl.h>
#include "crawler.cpp"

int main(int argc, char *argv[]){

    if ( ( argc != 5 ) && ( argc != 6 ) ) {
        std::cout << "usage: './main [url] [num_threads] [max_size] [FineBST | SetList] -s'\n";
        return 1;
    }
    if (! ((std::string) argv[4] == "FineBST") &&  ! ((std::string) argv[4] == "SetList")){
        std::cout << "usage: './main [url] [num_threads] [max_size] [FineBST | SetList] -s'\n";
        exit(1);
    }
    HtmlLink firstlink = HtmlLink(argv[1]);
    int no_threads = atoi(argv[2]);
    int max_size = atoi(argv[3]);
    bool to_study = (argc == 6);

    SafeUnboundedQueue<HtmlLink> links;
    if ((std::string) argv[4] == "FineBST"){
        CrawlerStruct<FineBST> myCrawler( firstlink, no_threads, max_size );
        insert_multithread(myCrawler, to_study, no_threads);
        if (! to_study )
            myCrawler.print();
        }
    else{
        CrawlerStruct<SetList> myCrawler( firstlink, no_threads, max_size);
        insert_multithread(myCrawler, to_study, no_threads);
        if (! to_study )
            myCrawler.print();
    }


    return 0;
}
