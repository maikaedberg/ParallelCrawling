#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <algorithm>

#include <curl/curl.h>
#include "crawler.cpp"

int main(int argc, char *argv[]){

    if ( ( argc != 5 ) && ( argc != 6 ) ) {
        std::cout << "usage: './main [url] [num_threads] [max_size] [settype] -s'\n";
        return 1;
    }

    std::string firstLink = argv[1];
    int num_threads = atoi(argv[2]);
    int max_size = atoi(argv[3]);
    if ((std::string) argv[4] == "FineBST"){
        SetList LinkDirectory(max_size);
    }
    if ((std::string) argv[4] == "SetList"){
        FineBST LinkDirectory;
    }

    bool to_study = (argc == 6);

    SetList LinkDirectory(max_size);
    SafeUnboundedQueue<std::string> links;

    curl_global_init(CURL_GLOBAL_ALL);
    
    insert_multithread(firstLink, num_threads, max_size, LinkDirectory, links, to_study);

    curl_global_cleanup();
    if (! to_study )
        LinkDirectory.print();

    return 0;
}
