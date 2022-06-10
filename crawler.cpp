#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <algorithm>
#include <cstdio>
#include <condition_variable>

#include <curl/curl.h>

#include "SetList.cpp"
#include "SafeUnboundedQueue.cpp"

std::condition_variable cv;
std::mutex lock;

static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream){
    // in this function, 
    // we take the ptr that points to where the file is written
    // replacing by blank space any of the parts of the file which is not a link
    // that is, only keeping 'link' where link is enclosed in <a*href="{link}"*</a>

    std::string htmlfile((char *) ptr, size * nmemb);
    std::string finallink = "";

    while (true){

        size_t begin_link = htmlfile.find("<a");
        if ( begin_link == std::string::npos )
            break;
        size_t end_link = htmlfile.find("</a>", begin_link+2);
        if (end_link == std::string::npos)
            break;
        std::string link_str = htmlfile.substr(begin_link + 2, end_link - begin_link - 2);

        size_t begin_ref = link_str.find("href=\"");
        if ( begin_ref == std::string::npos )
            break;
        size_t end_ref = link_str.find('"', begin_ref + 6);
        if ( end_ref == std::string::npos )
            break;
        finallink += ( link_str.substr(begin_ref + 6, end_ref - begin_ref - 6) + '\n' );
        
        htmlfile = htmlfile.substr(end_link + 4);
    }

    ((std::string*)stream)->append(finallink);
    return size * nmemb;

}

std::string crawl_website(std::string link){

    CURL *curl_handle;
    CURL *curl_share;

    curl_handle = curl_easy_init();
    //curl_share = curl_share_init();

    curl_easy_setopt(curl_handle, CURLOPT_URL, link.c_str());

    //curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 1L);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);

    curl_share = curl_share_init();
    curl_share_setopt(curl_share, CURLSHOPT_LOCKFUNC, CURL_LOCK_DATA_CONNECT);
    curl_share_setopt(curl_share, CURLSHOPT_UNLOCKFUNC, CURL_LOCK_DATA_CONNECT);
    
    // writes to linksFound the new blanked out source files
    std::string linksFound;

    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &linksFound);


    CURLcode res;
    res = curl_easy_perform(curl_handle);
    if (res != CURLE_OK){
        std::cout << "an error occured, code " << res << " for " << link << std::endl;
        exit(1);
    }

    curl_easy_cleanup(curl_handle);

    return linksFound;
 
}

std::string readLink(std::string firstLink, std::string link){
    if ( link.find("https://")  == std::string::npos &&  link.find("http://")  == std::string::npos ){
        if (link.length() > 2){
            if ( ( link[0] == '/' ) && ( link[1] == '/' ) ){
                return "https:" + link;
            }
            return firstLink + link;
        }
        else
            return "";
    }
    return link;
}

void crawl( SetList& LinkDirectory, SafeUnboundedQueue<std::string>& links, int max_size, bool verbose){

    while ( true ) { // there is still some links to treat

        auto start = std::chrono::steady_clock::now();

        std::string startlink = links.pop();

        if (startlink == ""){
            break;
        }

        auto start_crawl = std::chrono::steady_clock::now();
        std::string linksFound = crawl_website(startlink);
        auto finish_crawl = std::chrono::steady_clock::now();
        auto elapsed_crawl = std::chrono::duration_cast<std::chrono::microseconds>(finish_crawl - start_crawl).count();
            
        std::string currLink = "";

        for ( int i = 0; i < linksFound.length(); i++ ){

            if ( linksFound[i] == '\n'){
                std::string fullLink = readLink(startlink, currLink);
                if ( LinkDirectory.add(fullLink))
                    std::lock_guard<std::mutex> lk(lock);
                    if ( LinkDirectory.size() >= max_size ){
                        cv.notify_all();
                    }
                    links.push(fullLink);
                currLink = "";
            }
            else{
                currLink += linksFound[i];
            }
        }

        auto finish = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count();
        if ( verbose )
            printf("%zu %lli %lli\n", LinkDirectory.size(), elapsed, elapsed_crawl);
        links.decrementLinks();

    }

}

void insert_multithread(

    std::string firstLink, 
    int num_threads, 
    int max_size, 
    SetList& LinkDirectory,
    SafeUnboundedQueue<std::string>& links,
    bool verbose){


    if ( firstLink.find("https://")  == std::string::npos )
        firstLink = "http://" + firstLink;

    links.push(firstLink);
    LinkDirectory.add(firstLink);
    
    std::thread workers[num_threads];
    for (int i = 0; i < num_threads; i++){
        workers[i] = std::thread(&crawl, std::ref(LinkDirectory), std::ref(links), std::ref(max_size), verbose);
    }
    for (int i = 0; i < num_threads; i++){
        workers[i].detach();
    }

    std::unique_lock<std::mutex> lk(lock);
    cv.wait(lk);

    return;
}