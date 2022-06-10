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

Enjoy!
@maikaedberg

