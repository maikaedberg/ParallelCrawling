#include "DataStructure/CrawlerStruct.cpp"
#include <tuple>
#include <chrono>
#include <cstring>

template <typename T>
size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream){
    // link:= <a href="{link}"*

    std::string htmlfile((char *) ptr, size * nmemb);

    std::regex regexp_atg("<a href=\"[^ \t\r\n\v\f]*\"");
    std::smatch m; 

    while(regex_search(htmlfile, m, regexp_atg)){ 
        for (auto x : m){
            std::tuple<CrawlerStruct<T>, HtmlLink>* myInfo = (std::tuple<CrawlerStruct<T>, HtmlLink> *) stream ;
            CrawlerStruct<T> myCrawler = std::get<0>(*myInfo);
            HtmlLink firstLink = std::get<1>(*myInfo);
            myCrawler.add( HtmlLink ( firstLink, x) );
        }
        htmlfile = m.suffix() ;
    }

    return size * nmemb;

}


template <typename T>
void crawl_website(CrawlerStruct<T> myCrawler, HtmlLink startlink, bool verbose){
    CURL *curl_handle;
    curl_handle = curl_easy_init();

    curl_easy_setopt(curl_handle, CURLOPT_URL, startlink.url.c_str());
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, &write_data<T>);

    std::tuple<CrawlerStruct<T>, HtmlLink> myInfo = std::make_tuple(myCrawler, startlink);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &myInfo);
    auto start = std::chrono::system_clock::now();
    curl_easy_perform(curl_handle);

    auto finish = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count();
    if ( verbose )
        std::cout << "Crawler " << myCrawler.size() << " " << elapsed << std::endl;

    curl_easy_cleanup(curl_handle);
 
}

template <typename T>
void crawl(CrawlerStruct<T>& myCrawler, bool verbose){

    while ( ! myCrawler.isFull() ) {

        auto start = std::chrono::system_clock::now();

        HtmlLink startLink = myCrawler.pop();
        if ( startLink.isempty() ){
            break;
        }
        crawl_website<T>(myCrawler, startLink, verbose);
        
        myCrawler.decrementLinks();

        auto finish = std::chrono::system_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count();
        if ( verbose )
            std::cout << "Overall " << myCrawler.size() << " " << elapsed << std::endl;
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