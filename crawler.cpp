#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <algorithm>
#include <cstdio>
#include <regex>
#include <curl/curl.h>

#include "CrawlerStruct.cpp"


static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream){
    // link:= <a href="{link}"*
    
    std::string htmlfile((char *) ptr, size * nmemb);
    std::regex regexp_atg("<a href=\"[^ \t\r\n\v\f]*\"");
    std::smatch m; 
    
    while(regex_search(htmlfile, m, regexp_atg)){ 
        for (auto x : m){
            ((std::vector<std::string>*)stream)->push_back(x);
        }
        htmlfile = m.suffix() ;
    }

    return size * nmemb;

}

std::vector<std::string> crawl_website(HtmlLink link){

    CURL *curl_handle;
    curl_handle = curl_easy_init();

    curl_easy_setopt(curl_handle, CURLOPT_URL, link.url.c_str());
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);

    std::vector<std::string> linksFound;
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &linksFound);

    CURLcode res;
    res = curl_easy_perform(curl_handle);
    if (res != CURLE_OK){
        std::cout << "an error occured, code " << res << " for " << link.url << std::endl;
        exit(1);
    }

    curl_easy_cleanup(curl_handle);

    return linksFound;
 
}

template <typename T>
void crawl( T& LinkDirectory, SafeUnboundedQueue<HtmlLink>& links, int max_size, bool verbose){

    while ( LinkDirectory.count < max_size ) { // there is still some links to treat

        auto start = std::chrono::steady_clock::now();

        HtmlLink startLink = links.pop();
        if (startLink.isempty()){
            break;
        }

        auto start_crawl = std::chrono::steady_clock::now();
        std::vector<std::string> linksFound = crawl_website(startLink);
        auto finish_crawl = std::chrono::steady_clock::now();
        auto elapsed_crawl = std::chrono::duration_cast<std::chrono::microseconds>(finish_crawl - start_crawl).count();
    
        for (auto i : linksFound){
            HtmlLink link(startLink, i);
            links.push(link);
            LinkDirectory.add(link.url);
        }

        auto finish = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count();
        if ( verbose )
            printf("%i %lli %lli\n", LinkDirectory.count, elapsed, elapsed_crawl);
        links.decrementLinks();

    }

}

template <typename T>
void insert_multithread(

    HtmlLink firstLink, 
    int num_threads, 
    int max_size, 
    T& LinkDirectory,
    SafeUnboundedQueue<HtmlLink>& links,
    bool verbose){

    links.push(firstLink);
    LinkDirectory.add(firstLink.url);
    
    curl_global_init(CURL_GLOBAL_ALL);
    
    std::thread workers[num_threads];
    for (int i = 0; i < num_threads; i++){
        workers[i] = std::thread(&crawl<T>, std::ref(LinkDirectory), std::ref(links), std::ref(max_size), verbose);
    }
    for (int i = 0; i < num_threads; i++){
        workers[i].join();
    }
    
    
    curl_global_cleanup();

}