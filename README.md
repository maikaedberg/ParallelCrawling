# ParallelCrawling
Are you ready to learn to crawl?

It's simple.
First, install curl with
```
$ brew install curl
```
You can find more info [here](https://formulae.brew.sh/formula/curl).

Download this code and
If you want to crawl a specific website,  run
```
$ cd crawler
$ make main & ./main [url] [num_threads] [max_size]
```
The directory bounded at max_size of your url should be printed!

Otherwise, you can also visualiza how the code performs across different number of threads and insertions.
You can visualize it and plot it in the file ./benchmarker/plot_times.ipynb

Errors-
Libcurl has special error [codes](https://curl.se/libcurl/c/libcurl-errors.html) which are printed when something goes wrong.
If you get curl error 60, 
1. Download cacert.pm from this link: https://curl.se/docs/caextract.html.
2. In your php.ini, put this file location in the curl section, writing curl.cainfo = {your file location}

Enjoy!
@maikaedberg


Enjoy!
@maikaedberg

