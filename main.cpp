#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <algorithm>

#include <curl/curl.h>

#include "SetList.cpp"
#include "SafeUnboundedQueue.cpp"

static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream){
    // in this function, 
    // we take the ptr that points to where the file is written
    // replacing by blank space any of the parts of the file which is not a link
    // that is, only keeping 'link' where link is enclosed in href="{link}"

    bool towrite = false;
    char tofind[] = "href=\"";
    int ind = 0;

    for (int i = 0; i < nmemb; i++){
        char* curr = (char*)ptr + i*size;
        if (towrite){
            if (*curr == '"') {
                *curr = ' ';
                towrite = false;
            }
        }
        else{
            if (*curr == tofind[ind]){
                ind++;
            }
            else{
                ind = 0;
            }
            if (ind == 6){
                towrite = true; ind = 0;
            }
            *curr = ' ';
        }
    }

    ((std::string*)stream)->append((char*)ptr, size * nmemb);
    return size * nmemb;

}

std::string crawl_website(std::string link){

    CURL *curl_handle;

    curl_global_init(CURL_GLOBAL_ALL);
    curl_handle = curl_easy_init();

    curl_easy_setopt(curl_handle, CURLOPT_URL, link.c_str());
    //curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 1L);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);
    
    // writes to linksFound the new blanked out source files
    std::string linksFound;
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &linksFound);

    CURLcode res;
    res = curl_easy_perform(curl_handle);
    if (res != CURLE_OK){
        std::cout << "an error occured, " << link << " is not a valid link" << std::endl;
        exit(1);
    }

    curl_easy_cleanup(curl_handle);
    curl_global_cleanup();

    return linksFound;
 
}

std::string readLink(std::string firstLink, std::string link){
    if ( link.find("https://")  != std::string::npos ){
        return link;
    }
    return firstLink + link;
}

void crawl( SetList& LinkDirectory, SafeUnboundedQueue<std::string>& links, int max_size){

    while ( LinkDirectory.size() < max_size ) { // there is still some links to treat
        std::string link = links.pop();
        if (link == ""){
            break;
        }

        std::string linksFound = crawl_website(link);
        std::string currLink = "";

        for ( int i = 0; i < linksFound.length(); i++ ){
            if ( linksFound[i] != ' ' && linksFound[i] != '\n'){
                currLink += linksFound[i];
            }
            else {
                if (currLink != ""){
                    std::string fullLink = readLink(link, currLink);
                    bool added = LinkDirectory.add(fullLink);
                    if ( added )
                        links.push(fullLink);
                }
                currLink = "";
            }
        }

        links.decrementLinks();
        LinkDirectory.print();
    }

}

int main(int argc, char *argv[]){
    if( argc != 3 ) {
        std::cout << "usage: './main [url] [max_size]'\n";
        return 1;
    }
    SetList LinkDirectory;
    SafeUnboundedQueue<std::string> links;
    std::string firstLink = argv[1];
    if ( firstLink.find("https://")  == std::string::npos )
        firstLink = "https://" + firstLink;


    int max_size = atoi(argv[2]);

    links.push(firstLink);
    LinkDirectory.add(firstLink);

    int num_threads = 3;
    std::thread workers[num_threads];
    for (int i = 0; i < num_threads; i++){
        workers[i] = std::thread(&crawl, std::ref(LinkDirectory), std::ref(links), std::ref(max_size));
    }
    for (int i = 0; i < num_threads; i++){
        workers[i].join();
    }

    LinkDirectory.print();

    return 0;
}