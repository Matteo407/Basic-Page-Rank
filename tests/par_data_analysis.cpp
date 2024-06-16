#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <tuple>
#include <chrono>
#include <cmath>
#include <numeric>

#include "../src/par_page_rank.cpp"


int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cout << "Usage: " << argv[0] << " <graph_file> <num_threads>" << std::endl;
        return 1;
    }

    const char *filename = argv[1];
    const int cores = atoi(argv[2]);

    // Load the graph
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<parallel::CSC_Matrix*> matrices = parallel::load_graph_CSC(filename, cores);
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> duration = end - start;
    std::cout << "Time to load the file: " << duration.count() << " s" << std::endl << std::endl;
    
    // Print the matrix
    (*matrices[0]).print();
    (*matrices[0]).print_info();
    std::cout << std::endl;

    // Measure the mean execution time of the page rank algorithm
    double elapsed, mean_elapsed, max_elapsed, min_elapsed;
    for (int i = 0; i < 10; i++) {
        start = std::chrono::high_resolution_clock::now();
        std::vector<double> *result = parallel::Page_Rank(matrices, cores);
        end = std::chrono::high_resolution_clock::now();
        duration = end - start;
        elapsed = duration.count();
        std::cout << "Time: " << elapsed << " s" << std::endl;

        mean_elapsed += elapsed / 10;

        if (i == 0) {
            max_elapsed = elapsed;
            min_elapsed = elapsed;
        } else {
            if (elapsed > max_elapsed) max_elapsed = elapsed;
            if (elapsed < min_elapsed) min_elapsed = elapsed;
        }
    }
    
    std::cout << "Mean execution time: (" << mean_elapsed << " \\pm " << (max_elapsed - min_elapsed)/2 << ") " << " s" << std::endl;

    return 0;
}
