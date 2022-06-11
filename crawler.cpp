#include "CrawlerStruct.cpp"

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


std::vector<std::string> crawl_website(HtmlLink link){

    CURL *curl_handle;
    curl_handle = curl_easy_init();

    curl_easy_setopt(curl_handle, CURLOPT_URL, link.url.c_str());
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, &write_data);

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
void crawl(CrawlerStruct<T>& myCrawler, bool verbose){

    while ( ! myCrawler.isFull() ) { 

        HtmlLink startLink = myCrawler.pop();
        if (startLink.isempty()){
            break;
        }

        std::vector<std::string> linksFound = crawl_website(startLink);
       
        for (auto i : linksFound){
            myCrawler.add(HtmlLink(startLink, i));
        }

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