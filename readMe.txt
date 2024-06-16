# Sequential and parallel implementation of Page Rank
# Assignment for a university course

## Directories

-datagen: contains the file that defines the matrix data structure and the functions used to load the graph informations from the input file (in edgelist format).
It also contains an example file downloaded from the site https://snap.stanford.edu/data/index.html

-src: contains the files with the functions that calculate the actual Page Rank algorithm

-tests: contains a few example implementations of the whole algorithm. The main function loads the graph, initializes a random vector and finally prints out the Page Rank result, and the time taken to execute the algorithm.
        There are two files for both the sequential and parallel implementation: the one called for example "seq_test.cpp" executes the sequential algorithm a single time, while the one called "seq_data_analysis.cpp" measures the average time over various runs.

## Usage

The relevant files to compile in order to test the project are the ones in the "tests" directory.
The ones with suffix "par_", containing the parallel implementation, need to be compiled with the flag -fopenmp.
It is suggested to also always use the flag -O3 for increased efficiency.

Each of these files needs as a command-line argument the path to the input file, so the usage would be for example:
-compile:   g++ seq_test.cpp -O3 -o seq_test.exe
-run:       seq_test.exe <path-to-file>

In the case of the parallel version, also the number of cores must be specified, e.g.:
-compile:   g++ par_test.cpp -O3 -o par_test.exe
-run:       par_test.exe <path-to-file> <number-of-processors>
