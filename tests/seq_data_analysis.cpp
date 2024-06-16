/**
 * @file main.cpp
 * @brief This file contains the implementation of a program that loads a graph from a file and represents it using the Compressed Sparse Column (CSC) format.
 * It also provides functions to access and print the matrix elements.
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <tuple>
#include <chrono>
#include <cmath>
#include <numeric>

#include "../src/seq_page_rank.cpp"

// g++ -O3 main.cpp -o main
// "C:\Users\matte\OneDrive - unive.it\Unive\Cs 1 - Learning with massive data\Code implementations\Assignment 1 - Page Rank\datagen\test_file.txt"


// ------------------ Main ------------------
int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <graph_file>" << std::endl;
        return 1;
    }

    const char *filename = argv[1];

    auto start = std::chrono::high_resolution_clock::now();
    sequential::CSC_Matrix *M = sequential::load_graph_CSC(filename);
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> duration = end - start;
    std::cout << "Time to load the file: " << duration.count() << " s" << std::endl << std::endl;
    M->print(); M->print_info(); 

    // Measure the mean execution time of the page rank algorithm
    double elapsed, mean_elapsed, max_elapsed, min_elapsed;

    for (int i = 0; i < 15; i++) {
        start = std::chrono::high_resolution_clock::now();
        std::vector<double> *result = sequential::Page_Rank(M);
        end = std::chrono::high_resolution_clock::now();
        duration = end - start;
        elapsed = duration.count();
        std::cout << "Time: " << elapsed << " s" << std::endl;

        mean_elapsed += elapsed / 15;

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

