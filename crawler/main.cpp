#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <algorithm>

#include <curl/curl.h>

#include "../crawler.cpp"


int main(int argc, char *argv[]){

    if( argc != 4 ) {
        std::cout << "usage: './main [url] [num_threads] [max_size]'\n";
        return 1;
    }

    SetList LinkDirectory;
    SafeUnboundedQueue<std::string> links;

    std::string firstLink = argv[1];
    int num_threads = atoi(argv[2]);
    int max_size = atoi(argv[3]);

    curl_global_init(CURL_GLOBAL_ALL);
    
    insert_multithread(firstLink, num_threads, max_size, LinkDirectory, links);

    curl_global_cleanup();

    LinkDirectory.print();

    return 0;
}
