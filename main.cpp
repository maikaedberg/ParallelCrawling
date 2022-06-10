#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <algorithm>

#include <curl/curl.h>

#include "crawler.cpp"


int main(int argc, char *argv[]){

    if ( ( argc != 4 ) && ( argc != 5 ) ) {
        std::cout << "usage: './main [url] [num_threads] [max_size] -s'\n";
        return 1;
    }

    SetList LinkDirectory;
    SafeUnboundedQueue<std::string> links;

    std::string firstLink = argv[1];
    int num_threads = atoi(argv[2]);
    int max_size = atoi(argv[3]);
    bool to_study = (argc == 5);


    curl_global_init(CURL_GLOBAL_ALL);
    
    insert_multithread(firstLink, num_threads, max_size, LinkDirectory, links, to_study);

    curl_global_cleanup();
    if (! to_study )
        LinkDirectory.print();

    return 0;
}
