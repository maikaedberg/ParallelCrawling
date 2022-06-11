#include "CrawlerStruct.cpp"
#include <tuple>
#include <chrono>
#include <cstring>

size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream){
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

template <typename T>
void crawl_website(CrawlerStruct<T> myCrawler, HtmlLink startlink){
    CURL *curl_handle;
    curl_handle = curl_easy_init();

    curl_easy_setopt(curl_handle, CURLOPT_URL, startlink.url.c_str());
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, &write_data);

    std::vector<std::string> links;
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &links);
    auto start = std::chrono::system_clock::now();
    curl_easy_perform(curl_handle);
    for (auto link : links){
        myCrawler.add( HtmlLink( startlink, link) );
    }

    auto finish = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count();

    printf("%zu %lli\n", myCrawler.size(), elapsed);

    curl_easy_cleanup(curl_handle);
 
}

template <typename T>
void crawl(CrawlerStruct<T>& myCrawler, bool verbose){

    while ( ! myCrawler.isFull() ) { 

        HtmlLink startLink = myCrawler.pop();
        if ( startLink.isempty() ){
            break;
        }
        crawl_website<T>(myCrawler, startLink);

        myCrawler.decrementLinks();

    }

}

template <typename T>
void insert_multithread(CrawlerStruct<T>& myCrawler, bool verbose, int num_threads){

    curl_global_init(CURL_GLOBAL_ALL);
    
    std::thread workers[num_threads];
    for (int i = 0; i < num_threads; i++){
        workers[i] = std::thread(&crawl<T>, std::ref(myCrawler), verbose);
    }
    for (int i = 0; i < num_threads; i++){
        workers[i].join();
    }
    curl_global_cleanup();

}