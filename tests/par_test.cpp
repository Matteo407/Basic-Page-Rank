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
    std::vector<parallel::CSC_Matrix*> matrices = parallel::load_graph_CSC(filename, cores);
    
    // Print the matrix
    std::cout << "Matrix info:" << std::endl;
    (*matrices[0]).print();
    (*matrices[0]).print_info();
    std::cout << std::endl;


    // Run the Page Rank algorithm
    std::cout << "Running Page Rank" << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<double> *result = Page_Rank(matrices, cores);
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Page Rank completed" << std::endl;


    // Print the result
    std::cout << "v_result: [ ";
    for (int i = 0; i < 10; i++) {
        std::cout << (*result)[i] << " ";
    }
    std::cout << "]" << std::endl << std::endl;

    // Verify if it's still normalized 
    double sum = 0;
    for (long i = 0; i < matrices[0]->n; i++) {
        sum += (*result)[i];
    }
    std::cout << "Sum: " << sum << std::endl;

    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Time: " << elapsed.count() << " s" << std::endl;

    return 0;
}
