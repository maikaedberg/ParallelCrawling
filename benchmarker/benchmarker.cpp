
#include "../crawler.cpp"

int main(int argc, char *argv[]){

    if( argc != 5 ) {
        std::cout << "usage: './main [url] [num_threads] [max_size] [n_iter] \n";
        return 1;
    }

    SetList LinkDirectory;
    SafeUnboundedQueue<std::string> links;

    std::string firstLink = argv[1];
    int num_threads = atoi(argv[2]);
    int max_size = atoi(argv[3]);
    int n_iter = atoi(argv[4]);

    curl_global_init(CURL_GLOBAL_ALL);
    
    auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < n_iter; i++){
        int curr = (i + 1)*(max_size / n_iter);
        insert_multithread(firstLink, num_threads, curr, LinkDirectory, links);
        auto finish = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count();
        std::cout << curr << ' ' << elapsed << '\n';
    }

    curl_global_cleanup();

    return 0;
}