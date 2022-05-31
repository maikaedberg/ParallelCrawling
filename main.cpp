#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fstream>


 
#include <curl/curl.h>
#include "SetList.cpp"

static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream){


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

std::string linkFile(std::string file){
    file.erase(std::remove(file.begin(), file.end(), '/'), file.end());
    return file;
}



void crawl_website(const char* link){

    CURL *curl_handle;
    FILE *pagefile;
    curl_global_init(CURL_GLOBAL_ALL);
    curl_handle = curl_easy_init();
 
    curl_easy_setopt(curl_handle, CURLOPT_URL, link);
    //curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 1L);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);

    std::string file = (std::string) link + ".links";
    file = linkFile(file);

    static const char *pagefilename = file.c_str();

    pagefile = fopen(pagefilename, "wb");
    if (pagefile) {
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, pagefile);
        curl_easy_perform(curl_handle);
        fclose(pagefile);
    }
 
  curl_easy_cleanup(curl_handle);
 
  curl_global_cleanup();
 
}

int main(){
    SetList LinkDirectory = SetList();
    const char* link = "https://www.wikipedia.org/";
    crawl_website(link);

    std::string pagefile = link;
    pagefile = linkFile(pagefile);
    pagefile += ".links";

    std::string line;

    std::ifstream input_file(pagefile);
    if (input_file.is_open()) {
        while (getline(input_file, line)){
            if (line.substr(0,8) == "https://"){
                LinkDirectory->add(line);
            }
        }
    }

}