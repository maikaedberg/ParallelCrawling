#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <algorithm>

#include <curl/curl.h>

#include "SetList.cpp"
#include "SafeUnboundedQueue.cpp"

std::string filefromlink( std::string link){
    std::string file = link + ".links";
    std::string chars = "/;'";
    for (char c: chars) {
        file.erase(std::remove(file.begin(), file.end(), c), file.end());
    }
    return file;
}

static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream){
    //fwrite(ptr, size, nmemb, stdout);
    size_t written = 0;
    bool towrite = false;
    char tofind[] = {'h', 'r', 'e', 'f', '=', '"'} ;

    int ind = 0;

    for (int i = 0; i < nmemb; i++){
        char* curr = (char*)ptr + i*size;
        if (towrite){
            if (*curr == '"') {
                written+= fwrite("\n", size, 1, (FILE*) stream);
                towrite = false;
            }
            else{
                written+= fwrite(curr, size, 1, (FILE*) stream);
            }
        }
        else{
            written += sizeof(char);
            if (*curr == tofind[ind]){
                ind++;
            }
            else{
                ind = 0;
            }
            if (ind == 6){
                towrite = true; ind = 0;
            }
        }
    }
    return written;
}

void crawl_website(std::string link){
    CURL *curl_handle;
    FILE *pagefile;
    curl_global_init(CURL_GLOBAL_ALL);
    curl_handle = curl_easy_init();
 
    curl_easy_setopt(curl_handle, CURLOPT_URL, link.c_str());
    //curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 1L);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);

    std::string file = filefromlink(link);
    
    const char *pagefilename = file.c_str();
    pagefile = fopen(pagefilename, "wb");
    if (pagefile) {
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, pagefile);
        curl_easy_perform(curl_handle);
        fclose(pagefile);
    }
    else{
        std::cout << "something went wrong with creating " << pagefilename << '\n';
        exit(1);
    }
 
    curl_easy_cleanup(curl_handle);
    
    curl_global_cleanup();
 
}


void crawl( SetList& LinkDirectory, SafeUnboundedQueue<std::string>& links){

    while ( true ){ // there is still some links to treat
        std::string link = links.pop();
        crawl_website(link);

        std::string file = filefromlink(link);  

        std::string line;
        std::ifstream input_file(file);
        
        if ( input_file.is_open() ) {
            while ( getline(input_file, line) ){
                if ( line.substr(0,8) == "https://" ){
                    //std::cout << line << '\n';
                    bool added = LinkDirectory.add(line);
                    std::cout << line << '\n';
                    if ( added ){
                        links.push( line );
                    }
                }
            }
            remove( file.c_str() );
        }
        else{
            std::cout << "something went wrong with finding " << file << '\n';
            exit(1);
        }
    }
}

int main(){
    SetList LinkDirectory;
    SafeUnboundedQueue<std::string> links;
    std::string firstLink = "https://www.beachbunnymusic.com/";
    links.push(firstLink);
    LinkDirectory.add(firstLink);

    int num_threads = 4;
    std::thread workers[num_threads];
    for (int i = 0; i < num_threads; i++){
        workers[i] = std::thread(&crawl, std::ref(LinkDirectory), std::ref(links));
    }
    for (int i = 0; i < num_threads; i++){
        workers[i].join();
    }


    LinkDirectory.print();

}